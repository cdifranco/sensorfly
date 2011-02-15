%get cluster_name
clusters_name = zeros(1,size(center_new,1));
while 1
    current_sig = zeros(1,20);
    for j = 1 : 20
      current_sig(j) = get_sig_from_port(packet_id, serial_port, base_number); 
    end
    current_cluster = mode(current_sig);
    name_cluster = 0;
    while name_cluster < 1 || name_cluster > size(center_new,1)
        name_cluster = input('plz enter clusters name:');
    end
    clusters_name(current_cluster) = name_cluster;
    cont = input('still continue?');
    if cont == 0
       break;
    end
end