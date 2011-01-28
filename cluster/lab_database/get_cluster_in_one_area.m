% get signature from 'signatures'
function cluster_in_one_area = get_cluster_in_one_area(start_x, start_y, end_x, end_y, signatures)
cluster_in_one_area = [];
for width = start_x:10:end_x
    for length = start_y:10:end_y
        current_sig = signatures(5:end);
        cluster_in_one_area = [cluster_in_one_area, get_cluster_sig(center_sig, current_sig)];
    end
end
temp = [];
while size(cluster_in_one_area, 2) > 0
  [max count] = mode(cluster_in_one_area);
  temp = [temp, ones(1,count)*max];
  cluster_in_one_area(cluster_in_one_area(:) == mode(cluster_in_one_area))=[];
end
cluster_in_one_area = temp;