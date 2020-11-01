
#!/usr/bin/python

import json
import sys

#
# {
#	"name": "DeviceType",
#	"namesspace": "aquabox",
#   "values": [
#		"Filter"
#	]
# }
#
#


def writeToHeader(generatedLine):
	print (generatedLine)

if len(sys.argv) < 3:
	print("Template JSON file and output directory was not passed")
	exit(1)


enumToString = "namespa"

with open(sys.argv[1]) as template_json:
	template = json.load(template_json)
	enumName = template["name"]
	enumValues = template["values"]
	enumNamespace = template["namespace"]
	cppFile = "using namespace " + enumNamespace + ";\n\n"
	enumToString = "template<>\n" + enumName + "Serializable<" + enumName + ">::Enum2StrMap Serializable<" + enumName + ">::val2Str({\n"
	stringToEnum = "template<>\nSerializable<" + enumName + ">::Str2EnumMap Serializable<" + enumName + ">::str2Val({\n"
	for val in enumValues:
		enumToString += "     { " + enumName + "::" + val + " , \"" +  val + "\" }"
		stringToEnum += "     { \"" +  val + "\" , " + enumName + "::" + val + " }"
		postfix = ""
		if val is enumValues[-1]:
			postfix = "\n});"
		else:
			postfix = ",\n"

		enumToString += postfix
		stringToEnum += postfix

	cppFile += enumToString + "\n\n\n" + stringToEnum

print("Generated file: \n" + cppFile)

