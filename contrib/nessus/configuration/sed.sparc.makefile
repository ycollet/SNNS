/.*cat y.tab.c | sed -f sed.parser > co_parser.c.*$/ a\
\	\	cat co_parser.c | sed -f sed.sparc.parser > sed.tmp.parser\
\	\	mv sed.tmp.parser co_parser.c


