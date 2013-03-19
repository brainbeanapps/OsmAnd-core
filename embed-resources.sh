#!/bin/bash

SRCLOC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DEST=$SRCLOC/src/EmbeddedResources.cpp
exec > "$DEST"
exec 3> "$DEST"

echo -e "// AUTOGENERATED FILE" >&3
echo -e "#include \"RoutingConfiguration.h\"" >&3
echo -e "namespace OsmAnd {" >&3

SOURCE=$SRCLOC/routing.xml
echo -e "\tconst uint8_t RoutingConfiguration::_defaultRawXml[] = {" >&3
BYTE_COUNTER=0
exec 4<"$SOURCE"
IFS=
while read -u4 -d '' -r -s -n 1 byte
do
	if [ $((BYTE_COUNTER%16)) -eq 0 ]; then
		echo -e -n "\t\t" >&3
	fi
	printf "0x%02x, " "'$byte" >&3
	BYTE_COUNTER=$BYTE_COUNTER+1
	if [ $((BYTE_COUNTER%16)) -eq 0 ]; then
		echo "" >&3
	fi
done
exec 4<&-
echo "" >&3
echo -e "\t};" >&3
echo -e "\tconst size_t RoutingConfiguration::_defaultRawXmlSize = $((BYTE_COUNTER));" >&3

echo -e "} // namespace OsmAnd" >&3
exec 3>&-