#!/bin/sh
#
# get the C extern functions, compatible with C++.
#

cat <<__EOF__

#ifdef __cplusplus
extern "C" {
#endif
__EOF__

for i in "$@"; do
	echo
	echo "/* $i */"

#	a=$(grep -o '@fvar:\s*[A-Za-z0-9_]\+' $i \
#		|sed 's/@fvar:\s*//')
#	b=$(grep -v '@func:\s*[A-Za-z0-9_]\+\s*(static)' $i \
#		|grep -o '@func:\s*[A-Za-z0-9_]\+' \
#		|sed 's/@func:\s*//')
#	for j in $a $b; do
#		cat <<EOF
##undef $j
#EOF
#	done
#	for j in $a $b; do
#		cat <<EOF
##define $j $j
#EOF
#	done
#
#	echo

	sed -n '/^\/\* @fvar:/,/ = \|;$/p' "$i" \
	|sed 's/ = .*$/;/' \
	|sed '/\*\/$/aextern' \
	|sed '/^\/\*/,/\*\/$/d' \
	|sed 's/;/\n;/'

	sed '/^\/\* @func:.*(.*static.*)/,/\*\/$/d' "$i" \
	|sed -n '/^\/\* @func:/,/^{$\| {$/p' \
	|sed '/^\/\*/,/\*\/$/aextern' \
	|sed '/^\/\*/,/\*\/$/d' \
	|sed 's/^{$\| {$/;/' \
	|sed 's/\bnew\b/_new/g'
done

cat <<__EOF__

#ifdef __cplusplus
}
#endif

__EOF__
