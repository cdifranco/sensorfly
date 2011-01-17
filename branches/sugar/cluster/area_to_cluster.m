%get area_cluster
area_cluster_relation = [];
for i = 1 : area_number
   cluster_in_one_area = [];  
   for j = 1 : 20
      current_sig = get_sig_from_port(packet_id, serial_port, base_number); 
      cluster_in_one_area = [cluster_in_one_area, get_cluster_sig(center_sig, current_sig)]
   end
   for k =  1 : top
      area_cluster_relation = [area_cluster_relation, mode(cluster_in_one_area)];
      cluster_in_one_area(cluster_in_one_area(:) == mode(cluster_in_one_area))=[];
   end
   cont = input('still continue?');
   if cont == 0
       break;
   end
end