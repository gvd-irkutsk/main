def scatter_filter(t,z):
    C = 0.299 # m/ns
    C_w = 0.2148# m/ns
    coeff = 1
    i=0
    j=1
    t_res = []
    z_res = []
    print('t_diff, z_diff, t_diff*C, z[i], z[i+1], t[i], t[i+1]')
    while i<len(t)-1:
        if i+j >= len(t):
            break
        t_diff = abs(t[i] - t[i+j])
        z_diff = abs(z[i] - z[i+j])

#        if t_diff <= z_diff/C and 0 < z_diff <= 45:# and t_diff <= z_diff/(C_w*np.sin(np.pi-np.radians(82))):
        if (t_diff <= z_diff/(C_w*np.sin(np.pi-np.radians(82)))*coeff) and 0 <= z_diff <= 45:
            t_res.append(t[i])
            z_res.append(z[i])
            print(str(t[i])+'\t'+str(z[i])+' HIT')
#            j=1
        elif z_diff == 0:
            t_res.append(t[i])
            z_res.append(z[i])
            print(str(t[i])+'\t'+str(z[i])+' HIT')
            i+=1
        else:
            print(str(t[i])+'\t'+str(z[i])+' SCAT '+str(t_diff)+' > '+str(z_diff/(C_w*np.sin(np.pi-np.radians(82)))*coeff))
#            j+=1
        i+=1
    return t_res,z_res