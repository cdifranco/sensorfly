function [succ sig_route cluster_route coord_route start_cluster dest_cluster] = navigate(start_coord, dest_coord, trans_history, matrix, sigxy)
succ = 1;
success_count = 0;
total_count = 0;
direction_number = size(trans_history, 2);
current_coord = start_coord;
current_index = find(sigxy.x == start_coord(1) & sigxy.y == start_coord(2),1);
dest_index =  find(sigxy.x == dest_coord(1) & sigxy.y == dest_coord(2),1);
total_count = total_count + 1;
sig_route(total_count, 1:baseNumber) = current_coord;
coord_route(total_count, 1:2) = current_coord;
current_cluster = sigxy.cluster_id(current_index);
start_cluster = current_cluster;
dest_cluster = sigxy.cluster_id(dest_index);
cluster_route(total_count) = current_cluster;

while 1
    if sigxy.cluster_id(current_index) == dest_cluster
        break;
    else
        [path direction] = guide(currentSig, dest_cluster, trans_history, centers, matrix, base_number, distribution_table);
        if isempty(path)
            rand_direction = ceil(direction_number*rand);
            current_coord = get_next_step(rand_direction, current_coord, points);
        else
            current_coord = get_next_step(direction(1), current_coord, points);
        end
        total_count = total_count + 1;
        if total_count > 300
            succ = 0;
            break;
        end
        current_index = find(sigxy.x == start_coord(1) & sigxy.y == start_coord(2),1);
        sig_route(total_count, 1:baseNumber) = currentSig;
        coord_route(total_count, 1:2) = current_coord;
        current_cluster = get_cluster_sig(centers, currentSig, baseNumber, distribution_table);
        cluster_route(total_count) = current_cluster;
        if length(path) >= 1 && current_cluster == path(2)
            success_count = success_count + 1;
        end
    end
end