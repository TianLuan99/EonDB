# Protocol:

## Commands:
### get [key]
### set [key] [value]
### del [key]
```
+------+-----+------+-----+------+-----+-----+------+
| nstr | len | str1 | len | str2 | ... | len | strn |
+------+-----+------+-----+------+-----+-----+------+
```
### nstr is the number of strings, len is the length of following string, both are 32-bits integers.

#

## Response:
```
+-----+---------+
| res | data... |
+-----+---------+
```
### res is 32-bits status code followed by response string
