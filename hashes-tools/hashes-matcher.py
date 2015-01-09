import sys;

def get_list(fn):
    d = []
    fd = open(fn, 'rb')
    for l in fd:
        (cs, hash) = l.strip().split(';')
        d.append(hash)
    fd.close()
    return d

def get_dict(fn):
    d = {}
    fd = open(fn, 'rb')
    for l in fd:
        (cs, hash) = l.strip().split(';')
        id = int(cs.split('#')[1])
        if hash in d:
            d[hash].append(id)
        else:
            d[hash] = [id]
    fd.close()
    return d

def get_dict2(fn):
    d = {}
    fd = open(fn, 'rb')
    for l in fd:
        (cs, hash) = l.strip().split(';')
        id = int(cs.split('#')[1])
        if hash in d:
            d[hash] = (id, d[hash][1] + 1)
        else:
            d[hash] = (id, 1)
    fd.close()
    return d

dataf1 = get_list(sys.argv[1])
dataf2 = get_dict2(sys.argv[2])

#print dictf1
#print "~~~~~~~"
#print dictf2

idx = 0
for h in dataf1:
    if h in dataf2:
        print idx, dataf2[h]
    else:
        print idx, "########################################"
    idx = idx + 1

#print dictf1["a735a1fa75b0a2efa9e95d8b49f42d8b"]
#print dictf2["a735a1fa75b0a2efa9e95d8b49f42d8b"]
