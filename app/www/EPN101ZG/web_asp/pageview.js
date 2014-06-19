var pageview_length;
var pageview_pagesize;
var pageview_npages;
var pageview_current_page;
var showline_func;
var showheader_func;
var sort_by, sort_dir;
var pageview_list;
var pageview_showlist = new Array();
var pageview_table_div;
var pageview_have_checkbox;
var pageview_selected; 
var pageview_oncbxclick_func;
var pageview_row_color_func;
var pageview_title;

function pageview_item(title, index, width, cansort, mapfunc, linkfunc)
{
	this.title = title;
	this.index = index;
	this.width = width;
	this.cansort = cansort;
	this.mapfunc = mapfunc;
	this.linkfunc = linkfunc;
}

function pageview_add(title, index, width, cansort, mapfunc, linkfunc)
{
	pageview_showlist.length++;
	pageview_showlist[pageview_showlist.length-1]= new pageview_item(title, index, width, cansort, mapfunc, linkfunc);
}

function pageview_init(plist, size, tdiv, haveCbx, onCbxClick, rowColor, ptitle)
{
	var i;
	pageview_list = plist;
	pageview_title = ptitle;
	pageview_length = 0;
	for(i = 0; i < plist.length; i ++) if(plist[i] != '') pageview_length ++;
	pageview_pagesize = size;
	pageview_table_div = tdiv;
	pageview_have_checkbox = haveCbx;
	pageview_oncbxclick_func = onCbxClick;
	pageview_row_color_func = rowColor;
	pageview_selected = new Array(plist.length);
	for(var i = 0; i < plist.length; i ++) pageview_selected[i] = false;
	pageview_npages = Math.round(pageview_length / pageview_pagesize);
	if(pageview_length - pageview_npages * pageview_pagesize > 0) pageview_npages ++;
	pageview_current_page = 1;
	sort_by = -1;
	sort_dir = 1;
	pageview_goto(pageview_current_page);
}

function pageview_oncbxclick(item)
{
	pageview_selected[item] = document.getElementById('pageview_cb' + item).checked;
	if(pageview_oncbxclick_func)
		pageview_oncbxclick_func(item);
}

function pageview_clean_check(item, ch)
{
	pageview_selected[item] = ch;
	document.getElementById('pageview_cb' + item).checked = ch;
}

function pageview_goto(pn)
{
	var i, j, start, p;
	var str = '', link, vs;
	pageview_current_page = pn;
	str = '<table width=100% border=0><tr><td width=60% class=titleCell>' + pageview_title + '</td><td width=30%>';
	str += '<b class="page firefinder-match">Total: ' + pageview_length + '</b>';
	if(pageview_npages > 1) {
		for(var i = 1; i < pageview_npages + 1; i ++) {
			if(i == pageview_current_page) 
				str += '<a id="on" class="page" href="javascript:;">' + i + '</a>';
			else 
				str += '<a class="page" href="javascript:pageview_goto(' + i + ');">' + i + '</a>';
		}			
	}
	str += '</td></tr></table>';
	str += '<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>';
	str += '<tr>';
	if(pageview_have_checkbox) 
		str += '<td class=titleCell width=5%></td>';
	for(i = 0; i < pageview_showlist.length; i ++) {
		if(pageview_showlist[i].cansort)
			str += '<td class=titleCell width=' + pageview_showlist.width + 
			'>' + pageview_showlist[i].title + 
			sort_img(pageview_showlist[i].index) + '</td>';
		else
			str += '<td class=titleCell width=' + pageview_showlist.width + 
			'>' + pageview_showlist[i].title + '</td>';
	}
	str += '</tr>';
	
	var ncnt = 0;
	for(start = 0; start < pageview_list.length; start ++) {
		if(pageview_list[start] == '') continue;
		ncnt ++;
		if(ncnt > (pn - 1) * pageview_pagesize) break;
	}
	ncnt = 0;
	for(p = start;ncnt < pageview_pagesize;) {
//		p = start + i;
		if(p >= pageview_list.length) break;
		if(pageview_list[p] == '') {
			p ++;
			continue;
		}
		ncnt ++;
		vs = pageview_list[p].split(';');
		var bgc;
		if(pageview_row_color_func)
			bgc = pageview_row_color_func(p);
		else
			bgc = '#F0F0F0';
		str += '<tr>';
		if(pageview_have_checkbox) 
			str += '<td width=5% bgcolor="' + bgc + '"><input type="checkbox" id="pageview_cb' + p + '" ' + 
				'onclick="pageview_oncbxclick(' + p + ');"></td>';
		for(j = 0; j < pageview_showlist.length; j ++) {
			if(pageview_showlist[j].linkfunc) 
				link = '<a href="javascript:' + pageview_showlist[j].linkfunc +
					'(' + p + ');">';
			else
				link = '';
			if(pageview_showlist[j].mapfunc) 
				str += '<td width=' + pageview_showlist[j].width + ' bgcolor="' + bgc + '"' +
					'>' + link + pageview_showlist[j].mapfunc(vs[pageview_showlist[j].index]);
			else
				str += '<td width=' + pageview_showlist[j].width + ' bgcolor="' + bgc + '"' + 
					'>' + link + vs[pageview_showlist[j].index];
			if(link.length > 0)
				str += '</a></td>';
			else
				str += '</td>';
		}
		str += '</tr>';
		p ++;
	}
	str += '</table>';
	pageview_table_div.innerHTML = str;
}

function pageview_footer()
{
	document.write('<div id="pageview_footer_div" style="float: right; margin-top: 5px;"></div>');
}

function pageview_sort_asc(a, b)
{
	if(a == '') return -1;
	if(b == '') return 1;
	var s1 = a.split(';');
	var s2 = b.split(';');
	if(s1[sort_by] > s2[sort_by]) return -1;
	if(s1[sort_by] == s2[sort_by]) return 0;
	if(s1[sort_by] < s2[sort_by]) return 1;
}

function pageview_sort_dsc(a, b)
{
	if(a == '') return 1;
	if(b == '') return -1;
	var s1 = a.split(';');
	var s2 = b.split(';');
	if(s1[sort_by] > s2[sort_by]) return 1;
	if(s1[sort_by] == s2[sort_by]) return 0;
	if(s1[sort_by] < s2[sort_by]) return -1;
}

function pageview_sort(index)
{
	sort_by = index;
	if(sort_dir == 0) {
		pageview_list.sort(pageview_sort_asc);
		sort_dir = 1;
	}
	else {
		pageview_list.sort(pageview_sort_dsc);
		sort_dir = 0;
	}
	pageview_goto(1);
}

function sort_img(index)
{
	var str = '';
	str += '<a class="sort" href="javascript:pageview_sort(' + index  + ');">';
	if(sort_by == index) {
		if(sort_dir == 0) 
			str += '&nbsp;<img width=12 height=10 border=0 src="images/down_pointer.png"></img>';
		else 
			str += '&nbsp;<img width=12 height=10 border=0 src="images/up_pointer.png"></img>';
	}
	else
		str += '&nbsp;<img width=12 height=10 border=0 src="images/sort_none.png"></img>';
	str += '</a>';
	return str;
}

function pageview_ischecked(index)
{
	return pageview_selected[index];
}

function pageview_clearselected(index)
{
	pageview_selected[index] = false;
	if(document.getElementById('pageview_cb' + index)) document.getElementById('pageview_cb' + index).checked = false;
}
