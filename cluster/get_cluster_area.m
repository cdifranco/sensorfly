function cluster = get_cluster_area(reading, area, cluster_num, count_to_id)
cluster_count = zeros(1, cluster_num);
for i = 1:size(reading,1)
    if reading(i, 3) == area
        cluster_count(find(count_to_id == reading(i, 1))) = cluster_count(find(count_to_id == reading(i, 1))) + 1;
    end
end

[max_num cluster] = max(cluster_count);


