/*****************************************************
  tzlib.c, Tiny ZLIB.

  Code in this file was extracted from zlib-1.2.7.

  Only a small amout of code was modified/commented out
  to shrink binary size and to make compiler happy.
*****************************************************/

#include <preloader.h>
#include <zlib/zlib.h>
#include <zlib/zutil.h>
#include <zlib/inftrees.h>
#include <zlib/inflate.h>

#define MAXBITS 15

#define zmemcpy memcpy

#ifdef POSTINC
#  define OFF 0
#  define PUP(a) *(a)++
#else
#  define OFF 1
#  define PUP(a) *++(a)
#endif

#ifdef GUNZIP
/* 120606,JJ,seems gunzip uses NOT adler32(). */
/* #  define UPDATE(check, buf, len) \ */
/*     (state->flags ? crc32(check, buf, len) : adler32(check, buf, len)) */
#  define UPDATE(check, buf, len) crc32(check, buf, len)
#else
#  define UPDATE(check, buf, len) adler32(check, buf, len)
#endif

#ifdef GUNZIP
#  define CRC2(check, word)	  \
	do { \
		hbuf[0] = (unsigned char)(word); \
		hbuf[1] = (unsigned char)((word) >> 8); \
		check = crc32(check, hbuf, 2); \
	} while (0)

#  define CRC4(check, word)	  \
	do { \
		hbuf[0] = (unsigned char)(word); \
		hbuf[1] = (unsigned char)((word) >> 8); \
		hbuf[2] = (unsigned char)((word) >> 16); \
		hbuf[3] = (unsigned char)((word) >> 24); \
		check = crc32(check, hbuf, 4); \
	} while (0)
#endif

#define LOAD()	  \
	do { \
		put = strm->next_out; \
		left = strm->avail_out; \
		next = strm->next_in; \
		have = strm->avail_in; \
		hold = state->hold; \
		bits = state->bits; \
	} while (0)

#define NEEDBITS(n)	  \
	do { \
		while (bits < (unsigned)(n)) \
			PULLBYTE(); \
	} while (0)

#define BITS(n)	  \
	((unsigned)hold & ((1U << (n)) - 1))

#define DROPBITS(n)	  \
	do { \
		hold >>= (n); \
		bits -= (unsigned)(n); \
	} while (0)

#define PULLBYTE()	  \
	do { \
		if (have == 0) goto inf_leave; \
		have--; \
		hold += (unsigned long)(*next++) << bits; \
		bits += 8; \
	} while (0)

#define INITBITS()	  \
	do { \
		hold = 0; \
		bits = 0; \
	} while (0)

#define RESTORE()	  \
	do { \
		strm->next_out = put; \
		strm->avail_out = left; \
		strm->next_in = next; \
		strm->avail_in = have; \
		state->hold = hold; \
		state->bits = bits; \
	} while (0)

#define BYTEBITS()	  \
	do { \
		hold >>= bits & 7; \
		bits -= bits & 7; \
	} while (0)

static int ZEXPORT inflateResetKeep(strm)
     z_streamp strm;
{
	struct inflate_state FAR *state;

	if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
	state = (struct inflate_state FAR *)strm->state;
	strm->total_in = strm->total_out = state->total = 0;
	strm->msg = Z_NULL;
	if (state->wrap)        /* to support ill-conceived Java test suite */
		strm->adler = state->wrap & 1;
	state->mode = HEAD;
	state->last = 0;
	state->havedict = 0;
	state->dmax = 32768U;
	state->head = Z_NULL;
	state->hold = 0;
	state->bits = 0;
	state->lencode = state->distcode = state->next = state->codes;
	state->sane = 1;
	state->back = -1;
	return Z_OK;
}

static int ZEXPORT inflateReset(strm)
     z_streamp strm;
{
	struct inflate_state FAR *state;

	if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
	state = (struct inflate_state FAR *)strm->state;
	state->wsize = 0;
	state->whave = 0;
	state->wnext = 0;
	return inflateResetKeep(strm);
}

static int ZEXPORT inflateReset2(strm, windowBits)
     z_streamp strm;
     int windowBits;
{
	int wrap;
	struct inflate_state FAR *state;

	/* get the state */
	if (strm == Z_NULL || strm->state == Z_NULL) return Z_STREAM_ERROR;
	state = (struct inflate_state FAR *)strm->state;

	/* extract wrap request from windowBits parameter */
	if (windowBits < 0) {
		wrap = 0;
		windowBits = -windowBits;
	}
	else {
		wrap = (windowBits >> 4) + 1;
#ifdef GUNZIP
		if (windowBits < 48)
			windowBits &= 15;
#endif
	}

	/* set number of window bits, free window if different */
	if (windowBits && (windowBits < 8 || windowBits > 15))
		return Z_STREAM_ERROR;
	if (state->window != Z_NULL && state->wbits != (unsigned)windowBits) {
		ZFREE(strm, state->window);
		state->window = Z_NULL;
	}

	/* update state and reset the rest of it */
	state->wrap = wrap;
	state->wbits = (unsigned)windowBits;
	return inflateReset(strm);
}

static void ZLIB_INTERNAL inflate_fast(strm, start)
     z_streamp strm;
     unsigned start;         /* inflate()'s starting value for strm->avail_out */
{
	struct inflate_state FAR *state;
	unsigned char FAR *in;      /* local strm->next_in */
	unsigned char FAR *last;    /* while in < last, enough input available */
	unsigned char FAR *out;     /* local strm->next_out */
	unsigned char FAR *beg;     /* inflate()'s initial strm->next_out */
	unsigned char FAR *end;     /* while out < end, enough space available */
#ifdef INFLATE_STRICT
	unsigned dmax;              /* maximum distance from zlib header */
#endif
	unsigned wsize;             /* window size or zero if not using window */
	unsigned whave;             /* valid bytes in the window */
	unsigned wnext;             /* window write index */
	unsigned char FAR *window;  /* allocated sliding window, if wsize != 0 */
	unsigned long hold;         /* local strm->hold */
	unsigned bits;              /* local strm->bits */
	code const FAR *lcode;      /* local strm->lencode */
	code const FAR *dcode;      /* local strm->distcode */
	unsigned lmask;             /* mask for first level of length codes */
	unsigned dmask;             /* mask for first level of distance codes */
	code here;                  /* retrieved table entry */
	unsigned op;                /* code bits, operation, extra bits, or */
	/*  window position, window bytes to copy */
	unsigned len;               /* match length, unused bytes */
	unsigned dist;              /* match distance */
	unsigned char FAR *from;    /* where to copy match from */

	/* copy state to local variables */
	state = (struct inflate_state FAR *)strm->state;
	in = strm->next_in - OFF;
	last = in + (strm->avail_in - 5);
	out = strm->next_out - OFF;
	beg = out - (start - strm->avail_out);
	end = out + (strm->avail_out - 257);
#ifdef INFLATE_STRICT
	dmax = state->dmax;
#endif
	wsize = state->wsize;
	whave = state->whave;
	wnext = state->wnext;
	window = state->window;
	hold = state->hold;
	bits = state->bits;
	lcode = state->lencode;
	dcode = state->distcode;
	lmask = (1U << state->lenbits) - 1;
	dmask = (1U << state->distbits) - 1;

	/* decode literals and length/distances until end-of-block or not enough
	   input data or output space */
	do {
		if (bits < 15) {
			hold += (unsigned long)(PUP(in)) << bits;
			bits += 8;
			hold += (unsigned long)(PUP(in)) << bits;
			bits += 8;
		}
		here = lcode[hold & lmask];
	dolen:
		op = (unsigned)(here.bits);
		hold >>= op;
		bits -= op;
		op = (unsigned)(here.op);
		if (op == 0) {                          /* literal */
			PUP(out) = (unsigned char)(here.val);
		}
		else if (op & 16) {                     /* length base */
			len = (unsigned)(here.val);
			op &= 15;                           /* number of extra bits */
			if (op) {
				if (bits < op) {
					hold += (unsigned long)(PUP(in)) << bits;
					bits += 8;
				}
				len += (unsigned)hold & ((1U << op) - 1);
				hold >>= op;
				bits -= op;
			}
			if (bits < 15) {
				hold += (unsigned long)(PUP(in)) << bits;
				bits += 8;
				hold += (unsigned long)(PUP(in)) << bits;
				bits += 8;
			}
			here = dcode[hold & dmask];
		dodist:
			op = (unsigned)(here.bits);
			hold >>= op;
			bits -= op;
			op = (unsigned)(here.op);
			if (op & 16) {                      /* distance base */
				dist = (unsigned)(here.val);
				op &= 15;                       /* number of extra bits */
				if (bits < op) {
					hold += (unsigned long)(PUP(in)) << bits;
					bits += 8;
					if (bits < op) {
						hold += (unsigned long)(PUP(in)) << bits;
						bits += 8;
					}
				}
				dist += (unsigned)hold & ((1U << op) - 1);
#ifdef INFLATE_STRICT
				if (dist > dmax) {
					strm->msg = (char *)"invalid distance too far back";
					state->mode = BAD;
					break;
				}
#endif
				hold >>= op;
				bits -= op;
				op = (unsigned)(out - beg);     /* max distance in output */
				if (dist > op) {                /* see if copy from window */
					op = dist - op;             /* distance back in window */
					if (op > whave) {
						if (state->sane) {
							strm->msg =
								(char *)"invalid distance too far back";
							state->mode = BAD;
							break;
						}
#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
						if (len <= op - whave) {
							do {
								PUP(out) = 0;
							} while (--len);
							continue;
						}
						len -= op - whave;
						do {
							PUP(out) = 0;
						} while (--op > whave);
						if (op == 0) {
							from = out - dist;
							do {
								PUP(out) = PUP(from);
							} while (--len);
							continue;
						}
#endif
					}
					from = window - OFF;
					if (wnext == 0) {           /* very common case */
						from += wsize - op;
						if (op < len) {         /* some from window */
							len -= op;
							do {
								PUP(out) = PUP(from);
							} while (--op);
							from = out - dist;  /* rest from output */
						}
					}
					else if (wnext < op) {      /* wrap around window */
						from += wsize + wnext - op;
						op -= wnext;
						if (op < len) {         /* some from end of window */
							len -= op;
							do {
								PUP(out) = PUP(from);
							} while (--op);
							from = window - OFF;
							if (wnext < len) {  /* some from start of window */
								op = wnext;
								len -= op;
								do {
									PUP(out) = PUP(from);
								} while (--op);
								from = out - dist;      /* rest from output */
							}
						}
					}
					else {                      /* contiguous in window */
						from += wnext - op;
						if (op < len) {         /* some from window */
							len -= op;
							do {
								PUP(out) = PUP(from);
							} while (--op);
							from = out - dist;  /* rest from output */
						}
					}
					while (len > 2) {
						PUP(out) = PUP(from);
						PUP(out) = PUP(from);
						PUP(out) = PUP(from);
						len -= 3;
					}
					if (len) {
						PUP(out) = PUP(from);
						if (len > 1)
							PUP(out) = PUP(from);
					}
				}
				else {
					from = out - dist;          /* copy direct from output */
					do {                        /* minimum length is three */
						PUP(out) = PUP(from);
						PUP(out) = PUP(from);
						PUP(out) = PUP(from);
						len -= 3;
					} while (len > 2);
					if (len) {
						PUP(out) = PUP(from);
						if (len > 1)
							PUP(out) = PUP(from);
					}
				}
			}
			else if ((op & 64) == 0) {          /* 2nd level distance code */
				here = dcode[here.val + (hold & ((1U << op) - 1))];
				goto dodist;
			}
			else {
				strm->msg = (char *)"invalid distance code";
				state->mode = BAD;
				break;
			}
		}
		else if ((op & 64) == 0) {              /* 2nd level length code */
			here = lcode[here.val + (hold & ((1U << op) - 1))];
			goto dolen;
		}
		else if (op & 32) {                     /* end-of-block */
			state->mode = TYPE;
			break;
		}
		else {
			strm->msg = (char *)"invalid literal/length code";
			state->mode = BAD;
			break;
		}
	} while (in < last && out < end);

	/* return unused bytes (on entry, bits < 8, so in won't go too far back) */
	len = bits >> 3;
	in -= len;
	bits -= len << 3;
	hold &= (1U << bits) - 1;

	/* update state and return */
	strm->next_in = in + OFF;
	strm->next_out = out + OFF;
	strm->avail_in = (unsigned)(in < last ? 5 + (last - in) : 5 - (in - last));
	strm->avail_out = (unsigned)(out < end ?
	                             257 + (end - out) : 257 - (out - end));
	state->hold = hold;
	state->bits = bits;
	return;
}

local void fixedtables(state)
     struct inflate_state FAR *state;
{
#ifdef BUILDFIXED
	static int virgin = 1;
	static code *lenfix, *distfix;
	static code fixed[544];

	/* build fixed huffman tables if first call (may not be thread safe) */
	if (virgin) {
		unsigned sym, bits;
		static code *next;

		/* literal/length table */
		sym = 0;
		while (sym < 144) state->lens[sym++] = 8;
		while (sym < 256) state->lens[sym++] = 9;
		while (sym < 280) state->lens[sym++] = 7;
		while (sym < 288) state->lens[sym++] = 8;
		next = fixed;
		lenfix = next;
		bits = 9;
		inflate_table(LENS, state->lens, 288, &(next), &(bits), state->work);

		/* distance table */
		sym = 0;
		while (sym < 32) state->lens[sym++] = 5;
		distfix = next;
		bits = 5;
		inflate_table(DISTS, state->lens, 32, &(next), &(bits), state->work);

		/* do this just once */
		virgin = 0;
	}
#else /* !BUILDFIXED */
#   include <zlib/inffixed.h>
#endif /* BUILDFIXED */
	state->lencode = lenfix;
	state->lenbits = 9;
	state->distcode = distfix;
	state->distbits = 5;
}

local int updatewindow(strm, out)
     z_streamp strm;
     unsigned out;
{
	struct inflate_state FAR *state;
	unsigned copy, dist;

	state = (struct inflate_state FAR *)strm->state;

	/* if it hasn't been done already, allocate space for the window */
	if (state->window == Z_NULL) {
		state->window = (unsigned char FAR *)
			ZALLOC(strm, 1U << state->wbits,
			       sizeof(unsigned char));
		if (state->window == Z_NULL) return 1;
	}

	/* if window not in use yet, initialize */
	if (state->wsize == 0) {
		state->wsize = 1U << state->wbits;
		state->wnext = 0;
		state->whave = 0;
	}

	/* copy state->wsize or less output bytes into the circular window */
	copy = out - strm->avail_out;
	if (copy >= state->wsize) {
		zmemcpy(state->window, strm->next_out - state->wsize, state->wsize);
		state->wnext = 0;
		state->whave = state->wsize;
	}
	else {
		dist = state->wsize - state->wnext;
		if (dist > copy) dist = copy;
		zmemcpy(state->window + state->wnext, strm->next_out - copy, dist);
		copy -= dist;
		if (copy) {
			zmemcpy(state->window, strm->next_out - copy, copy);
			state->wnext = copy;
			state->whave = state->wsize;
		}
		else {
			state->wnext += dist;
			if (state->wnext == state->wsize) state->wnext = 0;
			if (state->whave < state->wsize) state->whave += dist;
		}
	}
	return 0;
}

static int ZEXPORT inflateEnd(z_streamp strm) {
	struct inflate_state FAR *state;

	if (strm == Z_NULL ||
	    strm->state == Z_NULL ||
	    strm->zfree == (free_func)0)
		return Z_STREAM_ERROR;

	state = (struct inflate_state FAR *)strm->state;

	if (state->window != Z_NULL) {
		ZFREE(strm, state->window);
	}

	ZFREE(strm, strm->state);

	strm->state = Z_NULL;

	return Z_OK;
}

static int ZEXPORT inflateInit2_(strm, windowBits, version, stream_size)
     z_streamp strm;
     int windowBits;
     const char *version;
     int stream_size;
{
	int ret;
	struct inflate_state FAR *state;

	if (version == Z_NULL || version[0] != ZLIB_VERSION[0] ||
	    stream_size != (int)(sizeof(z_stream)))
		return Z_VERSION_ERROR;
	if (strm == Z_NULL) return Z_STREAM_ERROR;
	strm->msg = Z_NULL;                 /* in case we return an error */
	/* 120607,JJ,zalloc and zfree are already set in decompress(). */
/* 	if (strm->zalloc == (alloc_func)0) { */
/* #ifdef Z_SOLO */
/* 		return Z_STREAM_ERROR; */
/* #else */
/* 		strm->zalloc = zcalloc; */
/* 		strm->opaque = (voidpf)0; */
/* #endif */
/* 	} */
/* 	if (strm->zfree == (free_func)0) */
/* #ifdef Z_SOLO */
/* 		return Z_STREAM_ERROR; */
/* #else */
/* 	strm->zfree = zcfree; */
/* #endif */
	state = (struct inflate_state FAR *)
		ZALLOC(strm, 1, sizeof(struct inflate_state));
	if (state == Z_NULL) return Z_MEM_ERROR;
	strm->state = (struct internal_state FAR *)state;
	state->window = Z_NULL;
	ret = inflateReset2(strm, windowBits);
	if (ret != Z_OK) {
		ZFREE(strm, state);
		strm->state = Z_NULL;
	}
	return ret;
}

#define TBLS 1

/* ========================================================================
 * Tables of CRC-32s of all single-byte values, made by make_crc_table().
 */
#include <crc32.h>

/* ========================================================================= */
#define DO1 crc = crc_table[0][((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

/* ========================================================================= */
/* 120607,JJ,adding static to crc32() increases code size. */
unsigned long ZEXPORT crc32(crc, buf, len)
    unsigned long crc;
    const unsigned char FAR *buf;
    uInt len;
{
    if (buf == Z_NULL) return 0UL;
    crc = crc ^ 0xffffffffUL;
    while (len >= 8) {
        DO8;
        len -= 8;
    }
    if (len) do {
        DO1;
    } while (--len);
    return crc ^ 0xffffffffUL;
}

static int ZEXPORT inflate(strm, flush)
     z_streamp strm;
     int flush;
{
	struct inflate_state FAR *state;
	unsigned char FAR *next;    /* next input */
	unsigned char FAR *put;     /* next output */
	unsigned have, left;        /* available input and output */
	unsigned long hold;         /* bit buffer */
	unsigned bits;              /* bits in bit buffer */
	unsigned in, out;           /* save starting available input and output */
	unsigned copy;              /* number of stored or match bytes to copy */
	unsigned char FAR *from;    /* where to copy match bytes from */
	code here;                  /* current decoding table entry */
	code last;                  /* parent table entry */
	unsigned len;               /* length to copy for repeats, bits to drop */
	int ret;                    /* return code */
#ifdef GUNZIP
	unsigned char hbuf[4];      /* buffer for gzip header crc calculation */
#endif
	static const unsigned short order[19] = /* permutation of code lengths */
		{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

	if (strm == Z_NULL || strm->state == Z_NULL || strm->next_out == Z_NULL ||
	    (strm->next_in == Z_NULL && strm->avail_in != 0))
		return Z_STREAM_ERROR;

	state = (struct inflate_state FAR *)strm->state;
	if (state->mode == TYPE) state->mode = TYPEDO;      /* skip check */
	LOAD();
	in = have;
	out = left;
	ret = Z_OK;
	for (;;)
		switch (state->mode) {
		case HEAD:
			if (state->wrap == 0) {
				state->mode = TYPEDO;
				break;
			}
			NEEDBITS(16);
#ifdef GUNZIP
			if ((state->wrap & 2) && hold == 0x8b1f) {  /* gzip header */
				state->check = crc32(0L, Z_NULL, 0);
				CRC2(state->check, hold);
				INITBITS();
				state->mode = FLAGS;
				break;
			}
			state->flags = 0;           /* expect zlib header */
			if (state->head != Z_NULL)
				state->head->done = -1;
			if (!(state->wrap & 1) ||   /* check if zlib header allowed */
			    ((BITS(8) << 8) + (hold >> 8)) % 31) {
				strm->msg = (char *)"incorrect header check";
				state->mode = BAD;
				break;
			}
#else
			/* 120606,JJ,the body of if is identical to the one above.
			 The reason to remain such a duplication is for editor's auto ident. */
			if (((BITS(8) << 8) + (hold >> 8)) % 31) {
				strm->msg = (char *)"incorrect header check";
				state->mode = BAD;
				break;
			}
#endif
			if (BITS(4) != Z_DEFLATED) {
				strm->msg = (char *)"unknown compression method";
				state->mode = BAD;
				break;
			}
			DROPBITS(4);
			len = BITS(4) + 8;
			if (state->wbits == 0)
				state->wbits = len;
			else if (len > state->wbits) {
				strm->msg = (char *)"invalid window size";
				state->mode = BAD;
				break;
			}
			state->dmax = 1U << len;
			/* 120606,JJ,seems gunzip uses NOT adler32(). */
			/* strm->adler = state->check = adler32(0L, Z_NULL, 0); */

			/* 120606,JJ,seems gunzip needs NOT DICTID. */
			/* state->mode = hold & 0x200 ? DICTID : TYPE; */
			state->mode = TYPE;
			INITBITS();
			break;
#ifdef GUNZIP
		case FLAGS:
			NEEDBITS(16);
			state->flags = (int)(hold);
			if ((state->flags & 0xff) != Z_DEFLATED) {
				strm->msg = (char *)"unknown compression method";
				state->mode = BAD;
				break;
			}
			if (state->flags & 0xe000) {
				strm->msg = (char *)"unknown header flags set";
				state->mode = BAD;
				break;
			}
			if (state->head != Z_NULL)
				state->head->text = (int)((hold >> 8) & 1);
			if (state->flags & 0x0200) CRC2(state->check, hold);
			INITBITS();
			state->mode = TIME;
		case TIME:
			NEEDBITS(32);
			if (state->head != Z_NULL)
				state->head->time = hold;
			if (state->flags & 0x0200) CRC4(state->check, hold);
			INITBITS();
			state->mode = OS;
		case OS:
			NEEDBITS(16);
			if (state->head != Z_NULL) {
				state->head->xflags = (int)(hold & 0xff);
				state->head->os = (int)(hold >> 8);
			}
			if (state->flags & 0x0200) CRC2(state->check, hold);
			INITBITS();
			state->mode = EXLEN;
		case EXLEN:
			if (state->flags & 0x0400) {
				NEEDBITS(16);
				state->length = (unsigned)(hold);
				if (state->head != Z_NULL)
					state->head->extra_len = (unsigned)hold;
				if (state->flags & 0x0200) CRC2(state->check, hold);
				INITBITS();
			}
			else if (state->head != Z_NULL)
				state->head->extra = Z_NULL;
			state->mode = EXTRA;
		case EXTRA:
			if (state->flags & 0x0400) {
				copy = state->length;
				if (copy > have) copy = have;
				if (copy) {
					if (state->head != Z_NULL &&
					    state->head->extra != Z_NULL) {
						len = state->head->extra_len - state->length;
						zmemcpy(state->head->extra + len, next,
						        len + copy > state->head->extra_max ?
						        state->head->extra_max - len : copy);
					}
					if (state->flags & 0x0200)
						state->check = crc32(state->check, next, copy);
					have -= copy;
					next += copy;
					state->length -= copy;
				}
				if (state->length) goto inf_leave;
			}
			state->length = 0;
			state->mode = NAME;
		case NAME:
			if (state->flags & 0x0800) {
				if (have == 0) goto inf_leave;
				copy = 0;
				do {
					len = (unsigned)(next[copy++]);
					if (state->head != Z_NULL &&
					    state->head->name != Z_NULL &&
					    state->length < state->head->name_max)
						state->head->name[state->length++] = len;
				} while (len && copy < have);
				if (state->flags & 0x0200)
					state->check = crc32(state->check, next, copy);
				have -= copy;
				next += copy;
				if (len) goto inf_leave;
			}
			else if (state->head != Z_NULL)
				state->head->name = Z_NULL;
			state->length = 0;
			state->mode = COMMENT;
		case COMMENT:
			if (state->flags & 0x1000) {
				if (have == 0) goto inf_leave;
				copy = 0;
				do {
					len = (unsigned)(next[copy++]);
					if (state->head != Z_NULL &&
					    state->head->comment != Z_NULL &&
					    state->length < state->head->comm_max)
						state->head->comment[state->length++] = len;
				} while (len && copy < have);
				if (state->flags & 0x0200)
					state->check = crc32(state->check, next, copy);
				have -= copy;
				next += copy;
				if (len) goto inf_leave;
			}
			else if (state->head != Z_NULL)
				state->head->comment = Z_NULL;
			state->mode = HCRC;
		case HCRC:
			if (state->flags & 0x0200) {
				NEEDBITS(16);
				if (hold != (state->check & 0xffff)) {
					strm->msg = (char *)"header crc mismatch";
					state->mode = BAD;
					break;
				}
				INITBITS();
			}
			if (state->head != Z_NULL) {
				state->head->hcrc = (int)((state->flags >> 9) & 1);
				state->head->done = 1;
			}
			strm->adler = state->check = crc32(0L, Z_NULL, 0);
			state->mode = TYPE;
			break;
#endif
		/* case DICTID: */
			/* 120606,JJ,seems gunzip needs NOT this case. */
			/* NEEDBITS(32); */
			/* strm->adler = state->check = ZSWAP32(hold); */
			/* INITBITS(); */
			/* state->mode = DICT; */
		/* case DICT: */
			/* 120606,JJ,seems gunzip needs NOT this case. */
			/* if (state->havedict == 0) { */
			/* 	RESTORE(); */
			/* 	return Z_NEED_DICT; */
			/* } */
			/* strm->adler = state->check = adler32(0L, Z_NULL, 0); */
			/* state->mode = TYPE; */
		case TYPE:
			if (flush == Z_BLOCK || flush == Z_TREES) goto inf_leave;
		case TYPEDO:
			if (state->last) {
				BYTEBITS();
				state->mode = CHECK;
				break;
			}
			NEEDBITS(3);
			state->last = BITS(1);
			DROPBITS(1);
			switch (BITS(2)) {
			case 0:                             /* stored block */
				state->mode = STORED;
				break;
			case 1:                             /* fixed block */
				fixedtables(state);
				state->mode = LEN_;             /* decode codes */
				if (flush == Z_TREES) {
					DROPBITS(2);
					goto inf_leave;
				}
				break;
			case 2:                             /* dynamic block */
				state->mode = TABLE;
				break;
			case 3:
				strm->msg = (char *)"invalid block type";
				state->mode = BAD;
			}
			DROPBITS(2);
			break;
		case STORED:
			BYTEBITS();                         /* go to byte boundary */
			NEEDBITS(32);
			if ((hold & 0xffff) != ((hold >> 16) ^ 0xffff)) {
				strm->msg = (char *)"invalid stored block lengths";
				state->mode = BAD;
				break;
			}
			state->length = (unsigned)hold & 0xffff;
			INITBITS();
			state->mode = COPY_;
			if (flush == Z_TREES) goto inf_leave;
		case COPY_:
			state->mode = COPY;
		case COPY:
			copy = state->length;
			if (copy) {
				if (copy > have) copy = have;
				if (copy > left) copy = left;
				if (copy == 0) goto inf_leave;
				zmemcpy(put, next, copy);
				have -= copy;
				next += copy;
				left -= copy;
				put += copy;
				state->length -= copy;
				break;
			}
			state->mode = TYPE;
			break;
		case TABLE:
			NEEDBITS(14);
			state->nlen = BITS(5) + 257;
			DROPBITS(5);
			state->ndist = BITS(5) + 1;
			DROPBITS(5);
			state->ncode = BITS(4) + 4;
			DROPBITS(4);
#ifndef PKZIP_BUG_WORKAROUND
			if (state->nlen > 286 || state->ndist > 30) {
				strm->msg = (char *)"too many length or distance symbols";
				state->mode = BAD;
				break;
			}
#endif
			state->have = 0;
			state->mode = LENLENS;
		case LENLENS:
			while (state->have < state->ncode) {
				NEEDBITS(3);
				state->lens[order[state->have++]] = (unsigned short)BITS(3);
				DROPBITS(3);
			}
			while (state->have < 19)
				state->lens[order[state->have++]] = 0;
			state->next = state->codes;
			state->lencode = (code const FAR *)(state->next);
			state->lenbits = 7;
			ret = inflate_table(CODES, state->lens, 19, &(state->next),
			                    &(state->lenbits), state->work);
			if (ret) {
				strm->msg = (char *)"invalid code lengths set";
				state->mode = BAD;
				break;
			}
			state->have = 0;
			state->mode = CODELENS;
		case CODELENS:
			while (state->have < state->nlen + state->ndist) {
				for (;;) {
					here = state->lencode[BITS(state->lenbits)];
					if ((unsigned)(here.bits) <= bits) break;
					PULLBYTE();
				}
				if (here.val < 16) {
					DROPBITS(here.bits);
					state->lens[state->have++] = here.val;
				}
				else {
					if (here.val == 16) {
						NEEDBITS(here.bits + 2);
						DROPBITS(here.bits);
						if (state->have == 0) {
							strm->msg = (char *)"invalid bit length repeat";
							state->mode = BAD;
							break;
						}
						len = state->lens[state->have - 1];
						copy = 3 + BITS(2);
						DROPBITS(2);
					}
					else if (here.val == 17) {
						NEEDBITS(here.bits + 3);
						DROPBITS(here.bits);
						len = 0;
						copy = 3 + BITS(3);
						DROPBITS(3);
					}
					else {
						NEEDBITS(here.bits + 7);
						DROPBITS(here.bits);
						len = 0;
						copy = 11 + BITS(7);
						DROPBITS(7);
					}
					if (state->have + copy > state->nlen + state->ndist) {
						strm->msg = (char *)"invalid bit length repeat";
						state->mode = BAD;
						break;
					}
					while (copy--)
						state->lens[state->have++] = (unsigned short)len;
				}
			}

			/* handle error breaks in while */
			if (state->mode == BAD) break;

			/* check for end-of-block code (better have one) */
			if (state->lens[256] == 0) {
				strm->msg = (char *)"invalid code -- missing end-of-block";
				state->mode = BAD;
				break;
			}

			/* build code tables -- note: do not change the lenbits or distbits
			   values here (9 and 6) without reading the comments in inftrees.h
			   concerning the ENOUGH constants, which depend on those values */
			state->next = state->codes;
			state->lencode = (code const FAR *)(state->next);
			state->lenbits = 9;
			ret = inflate_table(LENS, state->lens, state->nlen, &(state->next),
			                    &(state->lenbits), state->work);
			if (ret) {
				strm->msg = (char *)"invalid literal/lengths set";
				state->mode = BAD;
				break;
			}
			state->distcode = (code const FAR *)(state->next);
			state->distbits = 6;
			ret = inflate_table(DISTS, state->lens + state->nlen, state->ndist,
			                    &(state->next), &(state->distbits), state->work);
			if (ret) {
				strm->msg = (char *)"invalid distances set";
				state->mode = BAD;
				break;
			}
			state->mode = LEN_;
			if (flush == Z_TREES) goto inf_leave;
		case LEN_:
			state->mode = LEN;
		case LEN:
			if (have >= 6 && left >= 258) {
				RESTORE();
				inflate_fast(strm, out);
				LOAD();
				if (state->mode == TYPE)
					state->back = -1;
				break;
			}
			state->back = 0;
			for (;;) {
				here = state->lencode[BITS(state->lenbits)];
				if ((unsigned)(here.bits) <= bits) break;
				PULLBYTE();
			}
			if (here.op && (here.op & 0xf0) == 0) {
				last = here;
				for (;;) {
					here = state->lencode[last.val +
					                      (BITS(last.bits + last.op) >> last.bits)];
					if ((unsigned)(last.bits + here.bits) <= bits) break;
					PULLBYTE();
				}
				DROPBITS(last.bits);
				state->back += last.bits;
			}
			DROPBITS(here.bits);
			state->back += here.bits;
			state->length = (unsigned)here.val;
			if ((int)(here.op) == 0) {
				state->mode = LIT;
				break;
			}
			if (here.op & 32) {
				state->back = -1;
				state->mode = TYPE;
				break;
			}
			if (here.op & 64) {
				strm->msg = (char *)"invalid literal/length code";
				state->mode = BAD;
				break;
			}
			state->extra = (unsigned)(here.op) & 15;
			state->mode = LENEXT;
		case LENEXT:
			if (state->extra) {
				NEEDBITS(state->extra);
				state->length += BITS(state->extra);
				DROPBITS(state->extra);
				state->back += state->extra;
			}
			state->was = state->length;
			state->mode = DIST;
		case DIST:
			for (;;) {
				here = state->distcode[BITS(state->distbits)];
				if ((unsigned)(here.bits) <= bits) break;
				PULLBYTE();
			}
			if ((here.op & 0xf0) == 0) {
				last = here;
				for (;;) {
					here = state->distcode[last.val +
					                       (BITS(last.bits + last.op) >> last.bits)];
					if ((unsigned)(last.bits + here.bits) <= bits) break;
					PULLBYTE();
				}
				DROPBITS(last.bits);
				state->back += last.bits;
			}
			DROPBITS(here.bits);
			state->back += here.bits;
			if (here.op & 64) {
				strm->msg = (char *)"invalid distance code";
				state->mode = BAD;
				break;
			}
			state->offset = (unsigned)here.val;
			state->extra = (unsigned)(here.op) & 15;
			state->mode = DISTEXT;
		case DISTEXT:
			if (state->extra) {
				NEEDBITS(state->extra);
				state->offset += BITS(state->extra);
				DROPBITS(state->extra);
				state->back += state->extra;
			}
#ifdef INFLATE_STRICT
			if (state->offset > state->dmax) {
				strm->msg = (char *)"invalid distance too far back";
				state->mode = BAD;
				break;
			}
#endif
			state->mode = MATCH;
		case MATCH:
			if (left == 0) goto inf_leave;
			copy = out - left;
			if (state->offset > copy) {         /* copy from window */
				copy = state->offset - copy;
				if (copy > state->whave) {
					if (state->sane) {
						strm->msg = (char *)"invalid distance too far back";
						state->mode = BAD;
						break;
					}
#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
					copy -= state->whave;
					if (copy > state->length) copy = state->length;
					if (copy > left) copy = left;
					left -= copy;
					state->length -= copy;
					do {
						*put++ = 0;
					} while (--copy);
					if (state->length == 0) state->mode = LEN;
					break;
#endif
				}
				if (copy > state->wnext) {
					copy -= state->wnext;
					from = state->window + (state->wsize - copy);
				}
				else
					from = state->window + (state->wnext - copy);
				if (copy > state->length) copy = state->length;
			}
			else {                              /* copy from output */
				from = put - state->offset;
				copy = state->length;
			}
			if (copy > left) copy = left;
			left -= copy;
			state->length -= copy;
			do {
				*put++ = *from++;
			} while (--copy);
			if (state->length == 0) state->mode = LEN;
			break;
		case LIT:
			if (left == 0) goto inf_leave;
			*put++ = (unsigned char)(state->length);
			left--;
			state->mode = LEN;
			break;
		case CHECK:
			if (state->wrap) {
				NEEDBITS(32);
				out -= left;
				strm->total_out += out;
				state->total += out;
				if (out)
					strm->adler = state->check =
						UPDATE(state->check, put - out, out);
				out = left;
				if ((
#ifdef GUNZIP
				     state->flags ? hold :
#endif
				     ZSWAP32(hold)) != state->check) {
					strm->msg = (char *)"incorrect data check";
					state->mode = BAD;
					break;
				}
				INITBITS();
			}
#ifdef GUNZIP
			state->mode = LENGTH;
		case LENGTH:
			if (state->wrap && state->flags) {
				NEEDBITS(32);
				if (hold != (state->total & 0xffffffffUL)) {
					strm->msg = (char *)"incorrect length check";
					state->mode = BAD;
					break;
				}
				INITBITS();
			}
#endif
			state->mode = DONE;
		case DONE:
			ret = Z_STREAM_END;
			goto inf_leave;
		case BAD:
			ret = Z_DATA_ERROR;
			goto inf_leave;
		case MEM:
			return Z_MEM_ERROR;
		case SYNC:
		default:
			return Z_STREAM_ERROR;
		}

	/*
	  Return from inflate(), updating the total counts and the check value.
	  If there was no progress during the inflate() call, return a buffer
	  error.  Call updatewindow() to create and/or update the window state.
	  Note: a memory error from inflate() is non-recoverable.
	*/
 inf_leave:
	RESTORE();
	if (state->wsize || (out != strm->avail_out && state->mode < BAD &&
	                     (state->mode < CHECK || flush != Z_FINISH)))
		if (updatewindow(strm, out)) {
			state->mode = MEM;
			return Z_MEM_ERROR;
		}
	in -= strm->avail_in;
	out -= strm->avail_out;
	strm->total_in += in;
	strm->total_out += out;
	state->total += out;
	if (state->wrap && out)
		strm->adler = state->check =
			UPDATE(state->check, strm->next_out - out, out);
	strm->data_type = state->bits + (state->last ? 64 : 0) +
		(state->mode == TYPE ? 128 : 0) +
		(state->mode == LEN_ || state->mode == COPY_ ? 256 : 0);
	if (((in == 0 && out == 0) || flush == Z_FINISH) && ret == Z_OK)
		ret = Z_BUF_ERROR;
	return ret;
}

int ZLIB_INTERNAL inflate_table(type, lens, codes, table, bits, work)
     codetype type;
     unsigned short FAR *lens;
     unsigned codes;
     code FAR * FAR *table;
     unsigned FAR *bits;
     unsigned short FAR *work;
{
	unsigned len;               /* a code's length in bits */
	unsigned sym;               /* index of code symbols */
	unsigned min, max;          /* minimum and maximum code lengths */
	unsigned root;              /* number of index bits for root table */
	unsigned curr;              /* number of index bits for current table */
	unsigned drop;              /* code bits to drop for sub-table */
	int left;                   /* number of prefix codes available */
	unsigned used;              /* code entries in table used */
	unsigned huff;              /* Huffman code */
	unsigned incr;              /* for incrementing code, index */
	unsigned fill;              /* index for replicating entries */
	unsigned low;               /* low bits for current root entry */
	unsigned mask;              /* mask for low root bits */
	code here;                  /* table entry for duplication */
	code FAR *next;             /* next available space in table */
	const unsigned short FAR *base;     /* base value table to use */
	const unsigned short FAR *extra;    /* extra bits table to use */
	int end;                    /* use base and extra for symbol > end */
	unsigned short count[MAXBITS+1];    /* number of codes of each length */
	unsigned short offs[MAXBITS+1];     /* offsets in table for each length */
	static const unsigned short lbase[31] = { /* Length codes 257..285 base */
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
	static const unsigned short lext[31] = { /* Length codes 257..285 extra */
		16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18,
		19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 78, 68};
	static const unsigned short dbase[32] = { /* Distance codes 0..29 base */
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
		8193, 12289, 16385, 24577, 0, 0};
	static const unsigned short dext[32] = { /* Distance codes 0..29 extra */
		16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
		23, 23, 24, 24, 25, 25, 26, 26, 27, 27,
		28, 28, 29, 29, 64, 64};

	/*
	  Process a set of code lengths to create a canonical Huffman code.  The
	  code lengths are lens[0..codes-1].  Each length corresponds to the
	  symbols 0..codes-1.  The Huffman code is generated by first sorting the
	  symbols by length from short to long, and retaining the symbol order
	  for codes with equal lengths.  Then the code starts with all zero bits
	  for the first code of the shortest length, and the codes are integer
	  increments for the same length, and zeros are appended as the length
	  increases.  For the deflate format, these bits are stored backwards
	  from their more natural integer increment ordering, and so when the
	  decoding tables are built in the large loop below, the integer codes
	  are incremented backwards.

	  This routine assumes, but does not check, that all of the entries in
	  lens[] are in the range 0..MAXBITS.  The caller must assure this.
	  1..MAXBITS is interpreted as that code length.  zero means that that
	  symbol does not occur in this code.

	  The codes are sorted by computing a count of codes for each length,
	  creating from that a table of starting indices for each length in the
	  sorted table, and then entering the symbols in order in the sorted
	  table.  The sorted table is work[], with that space being provided by
	  the caller.

	  The length counts are used for other purposes as well, i.e. finding
	  the minimum and maximum length codes, determining if there are any
	  codes at all, checking for a valid set of lengths, and looking ahead
	  at length counts to determine sub-table sizes when building the
	  decoding tables.
	*/

	/* accumulate lengths for codes (assumes lens[] all in 0..MAXBITS) */
	for (len = 0; len <= MAXBITS; len++)
		count[len] = 0;
	for (sym = 0; sym < codes; sym++)
		count[lens[sym]]++;

	/* bound code lengths, force root to be within code lengths */
	root = *bits;
	for (max = MAXBITS; max >= 1; max--)
		if (count[max] != 0) break;
	if (root > max) root = max;
	if (max == 0) {                     /* no symbols to code at all */
		here.op = (unsigned char)64;    /* invalid code marker */
		here.bits = (unsigned char)1;
		here.val = (unsigned short)0;
		*(*table)++ = here;             /* make a table to force an error */
		*(*table)++ = here;
		*bits = 1;
		return 0;     /* no symbols, but wait for decoding to report error */
	}
	for (min = 1; min < max; min++)
		if (count[min] != 0) break;
	if (root < min) root = min;

	/* check for an over-subscribed or incomplete set of lengths */
	left = 1;
	for (len = 1; len <= MAXBITS; len++) {
		left <<= 1;
		left -= count[len];
		if (left < 0) return -1;        /* over-subscribed */
	}
	if (left > 0 && (type == CODES || max != 1))
		return -1;                      /* incomplete set */

	/* generate offsets into symbol table for each length for sorting */
	offs[1] = 0;
	for (len = 1; len < MAXBITS; len++)
		offs[len + 1] = offs[len] + count[len];

	/* sort symbols by length, by symbol order within each length */
	for (sym = 0; sym < codes; sym++)
		if (lens[sym] != 0) work[offs[lens[sym]]++] = (unsigned short)sym;

	/*
	  Create and fill in decoding tables.  In this loop, the table being
	  filled is at next and has curr index bits.  The code being used is huff
	  with length len.  That code is converted to an index by dropping drop
	  bits off of the bottom.  For codes where len is less than drop + curr,
	  those top drop + curr - len bits are incremented through all values to
	  fill the table with replicated entries.

	  root is the number of index bits for the root table.  When len exceeds
	  root, sub-tables are created pointed to by the root entry with an index
	  of the low root bits of huff.  This is saved in low to check for when a
	  new sub-table should be started.  drop is zero when the root table is
	  being filled, and drop is root when sub-tables are being filled.

	  When a new sub-table is needed, it is necessary to look ahead in the
	  code lengths to determine what size sub-table is needed.  The length
	  counts are used for this, and so count[] is decremented as codes are
	  entered in the tables.

	  used keeps track of how many table entries have been allocated from the
	  provided *table space.  It is checked for LENS and DIST tables against
	  the constants ENOUGH_LENS and ENOUGH_DISTS to guard against changes in
	  the initial root table size constants.  See the comments in inftrees.h
	  for more information.

	  sym increments through all symbols, and the loop terminates when
	  all codes of length max, i.e. all codes, have been processed.  This
	  routine permits incomplete codes, so another loop after this one fills
	  in the rest of the decoding tables with invalid code markers.
	*/

	/* set up for code type */
	switch (type) {
	case CODES:
		base = extra = work;    /* dummy value--not used */
		end = 19;
		break;
	case LENS:
		base = lbase;
		base -= 257;
		extra = lext;
		extra -= 257;
		end = 256;
		break;
	default:            /* DISTS */
		base = dbase;
		extra = dext;
		end = -1;
	}

	/* initialize state for loop */
	huff = 0;                   /* starting code */
	sym = 0;                    /* starting code symbol */
	len = min;                  /* starting code length */
	next = *table;              /* current table to fill in */
	curr = root;                /* current table index bits */
	drop = 0;                   /* current bits to drop from code for index */
	low = (unsigned)(-1);       /* trigger new sub-table when len > root */
	used = 1U << root;          /* use root table entries */
	mask = used - 1;            /* mask for comparing low */

	/* check available table space */
	if ((type == LENS && used >= ENOUGH_LENS) ||
	    (type == DISTS && used >= ENOUGH_DISTS))
		return 1;

	/* process all codes and make table entries */
	for (;;) {
		/* create table entry */
		here.bits = (unsigned char)(len - drop);
		if ((int)(work[sym]) < end) {
			here.op = (unsigned char)0;
			here.val = work[sym];
		}
		else if ((int)(work[sym]) > end) {
			here.op = (unsigned char)(extra[work[sym]]);
			here.val = base[work[sym]];
		}
		else {
			here.op = (unsigned char)(32 + 64);         /* end of block */
			here.val = 0;
		}

		/* replicate for those indices with low len bits equal to huff */
		incr = 1U << (len - drop);
		fill = 1U << curr;
		min = fill;                 /* save offset to next table */
		do {
			fill -= incr;
			next[(huff >> drop) + fill] = here;
		} while (fill != 0);

		/* backwards increment the len-bit code huff */
		incr = 1U << (len - 1);
		while (huff & incr)
			incr >>= 1;
		if (incr != 0) {
			huff &= incr - 1;
			huff += incr;
		}
		else
			huff = 0;

		/* go to next symbol, update count, len */
		sym++;
		if (--(count[len]) == 0) {
			if (len == max) break;
			len = lens[work[sym]];
		}

		/* create new sub-table if needed */
		if (len > root && (huff & mask) != low) {
			/* if first time, transition to sub-tables */
			if (drop == 0)
				drop = root;

			/* increment past last table */
			next += min;            /* here min is 1 << curr */

			/* determine length of next table */
			curr = len - drop;
			left = (int)(1 << curr);
			while (curr + drop < max) {
				left -= count[curr + drop];
				if (left <= 0) break;
				curr++;
				left <<= 1;
			}

			/* check for enough space */
			used += 1U << curr;
			if ((type == LENS && used >= ENOUGH_LENS) ||
			    (type == DISTS && used >= ENOUGH_DISTS))
				return 1;

			/* point entry in root table to sub-table */
			low = huff & mask;
			(*table)[low].op = (unsigned char)curr;
			(*table)[low].bits = (unsigned char)root;
			(*table)[low].val = (unsigned short)(next - *table);
		}
	}

	/* fill in remaining table entry if code is incomplete (guaranteed to have
	   at most one remaining entry, since if the code is incomplete, the
	   maximum code length that was allowed to get this far is one bit) */
	if (huff != 0) {
		here.op = (unsigned char)64;            /* invalid code marker */
		here.bits = (unsigned char)(len - drop);
		here.val = (unsigned short)0;
		next[huff] = here;
	}

	/* set return parameters */
	*table += used;
	*bits = root;
	return 0;
}

#define	ZALLOC_ALIGNMENT	16

void *heap_base;

static void *zalloc(void *x __attribute__((__unused__)),
                    unsigned items, unsigned size) {
	void *p = heap_base;

	size *= items;
	size = (size + ZALLOC_ALIGNMENT - 1) & ~(ZALLOC_ALIGNMENT - 1);

	heap_base += size;

	return (p);
}

static void zfree(void *x    __attribute__((__unused__)),
                  void *addr __attribute__((__unused__))) {
	return;
}

s32_t decompress(u8_t *src, u8_t *dest, void *heap) {
	int ret;
	z_stream strm;

	heap_base = heap;

	/* allocate inflate state */
	strm.zalloc   = zalloc;
	strm.zfree    = zfree;
	strm.avail_in = DEFLATED_UBOOT_SIZE;
	strm.next_in  = src;

	ret = inflateInit2(&strm, 16+MAX_WBITS);

	if (ret != Z_OK) return ret;

	/* The inflated size is stored at the last 4 bytes of the
	   deflated file.
	   Ref.: http://www.onicos.com/staff/iz/formats/gzip.html */
	strm.avail_out = endian_swap(src+DEFLATED_UBOOT_SIZE-4);
	strm.next_out = dest;

	ret = inflate(&strm, Z_FINISH);

	/* clean up and return */
	(void)inflateEnd(&strm);

	return ret;
}
