#!/bin/sh
#
# check the parameters header in the C files.
#

grep -nH '#[0-9rN]\+:' "$@" \
|grep -v '\(#[0-9]\+:\s*\(\(([^)]*)\)\|[A-za-z0-9_]\)\+\s*\[\(in/out\|out\|in\)\]\)\|\(#[rN]\s*:\s*\(([^)]*)\)\?\s*\[\(ret\|in/out\|out\|in\)\]\)'

for i in "$@"; do
	awk -F '\n' '
BEGIN {
	fname = "";
	fname_y = 0;
	fname_line = 0;
	parr[0];
	parr_y[0];
	parr_line[0];
	parr_n = 0;
	parr_nn = 0;

	st = 0;
}

function gmatch(s, m,    n)
{
	match(s, "\\<" m "\\>");
	if (RLENGTH == -1)
		return 0;

	n = 1;
	while (length(s)) {
		s = substr(s, RSTART + RLENGTH);
		match(s, "\\<" m "\\>");
		if (RLENGTH == -1)
			break;
		n++;
	}

	return n;
}

{
	s = $0;
	if (st == 0) {
		if (s ~ /^\/\* @func:\s*[A-Za-z0-9_]+/) {
			delete parr;
			delete parr_y;
			delete parr_line;

			sub(/^\/\* @func:\s*/, "", s);
			match(s, /[A-Za-z0-9_]+/);
			fname = substr(s, RSTART, RLENGTH);
			fname_y = 0;
			fname_line = NR;

			parr[0];
			parr_y[0];
			parr_line[0];
			parr_n = 0;
			parr_nn = 0;
			st = 1;
		}
	} else if (st == 1) {
		if (s ~ /^ \* #[0-9]+:/) {
			ss = s;
			sub(/^ \* #/, "", ss);
			sub(/:.*$/, "", ss);
			parr_nn++;
			if (parr_nn != ss) {
				print FILENAME;
				print "  order error ?line:" NR ": " s;
			}
		}

		if (s ~ /^ \* #[0-9]+:\s*[A-za-z0-9_]+/) {
			sub(/^ \* #[0-9]+:\s*/, "", s);
			sub(/\s*\[.*$/, "", s);
			parr[parr_n] = s;
			parr_y[parr_n] = 0;
			parr_line[parr_n] = NR;
			parr_n++;
		} else if (s ~ /^ \*\/$/) {
			for (k in parr) {
				n = 0;
				for (j in parr) {
					if (parr[k] == parr[j])
						n++;
				}
				if (n > 1) {
					print FILENAME;
					print "  repates ?line:" parr_line[k] ": " \
						fname "() #" k + 1 " " parr[k];
				}
			}
			st = 2;
		}
	} else if (st == 2) {
		if (fname) {
			if (gmatch(s, fname))
				fname_y = 1;
		}
		if (parr_n) {
			for (k in parr) {
				parr_y[k] += gmatch(s, parr[k]);
			}
		}

		if (s ~ /{$/) {
			if (!fname_y) {
				print FILENAME;
				print "  function not found ?line:" fname_line ": " \
					fname "()";
			}
			if (parr_n) {
				for (k in parr) {
					if (parr_y[k] > 1) {
						print FILENAME;
						print "  repates ?line:" parr_line[k] ": " \
							fname "() #" k + 1 " " parr[k];
					} else if (!parr_y[k]) {
						print FILENAME;
						print "  not matched ?line:" parr_line[k] ": " \
							fname "() #" k + 1 " " parr[k];
					}
				}
			}
			st = 0;
		}
	}
}
' "$i"
done
