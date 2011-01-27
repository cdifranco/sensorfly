% discarded, not used any more
function cluster = get_cluster(reading, area, cluster_num)

cluster_count = zeros(1, cluster_num);
for i = 1:size(reading,1)
    if reading(i, 3) == area
        cluster_count(reading(i, 1)) = cluster_count(reading(i, 1)) + 1;
    end
end

[~, cluster] = max(cluster_count);