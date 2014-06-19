#!/bin/bash
echo `svn info $1 | grep Revision | awk '{ print $2 }'`

