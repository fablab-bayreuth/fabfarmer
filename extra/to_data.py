import sys

if len(sys.argv) < 2:
    print "Usage: ", sys.argv[0], '<input_file>'
    exit(1)

data = open(sys.argv[1], 'rb').read()

strdata = 'static const uint8_t ' + sys.argv[1].replace('.','_') + '[] PROGMEM = {\n\t'
llen = 1
for d in data:
    strdata += '0x%.02x, ' % ord(d)
    llen += 1
    if llen >= 16:
        strdata += '\n\t'
        llen = 1
strdata = strdata[:-2]
strdata += '};\n'
        
outfile = open(sys.argv[1] + '.c', 'wb')
outfile.write(strdata)
outfile.close()
