#!/bin/sh
for xxx in `find web_html/*.htm`; do ./html2asp $xxx web_asp; done
