% get the cluster which the signature belongs to 
function cluster = get_cluster_sig(centers, sig, base_number, distribution_table)
dist_possibility = zeros(1, size(centers,1));
for j = 1 : size(centers,1)
    edist = 0;
    valid_reading_count = 0;
    for sc = 1 : base_number
        if sig(sc) ~= inf && centers(j, sc) ~= inf
            edist = edist + (sig(sc)-centers(j, sc)).^2;
            valid_reading_count = valid_reading_count + 1;
        end
    end
    if valid_reading_count < 4  % no such distribution table
        dist_possibility(j) = 0;
        continue;
    end
    edist = edist.^.5;
    dist_possibility(j) = possibility(edist,distribution_table{valid_reading_count});
end

[~, cluster] = max(dist_possibility);