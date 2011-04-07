function [succ cluster_route last_coord] = random_navigate(start_coord, dest_coord, trans_history, matrix, sigxy)
succ = 1;
total_count = 0;
direction_number = size(trans_history, 2);
current_coord = start_coord;
last_coord = current_coord;
current_index = find(sigxy.x == start_coord(1) & sigxy.y == start_coord(2),1);
dest_index =  find(sigxy.x == dest_coord(1) & sigxy.y == dest_coord(2),1);
total_count = total_count + 1;
current_cluster = sigxy.cluster_id(current_index);
dest_cluster = sigxy.cluster_id(dest_index);
cluster_route(total_count) = current_cluster;

while 1
    if sigxy.cluster_id(current_index) == dest_cluster
        last_coord = [sigxy.x(current_index), sigxy.y(current_index)];
        break;
    else
        rand_direction = ceil(direction_number*rand);
        current_coord = get_next_step(rand_direction, current_coord, sigxy);
        total_count = total_count + 1;
        if total_count > 500 % if more than 500 steps, count it as fail
            succ = 0;
            break;
        end
        current_index = find(sigxy.x == current_coord(1) & sigxy.y == current_coord(2),1);
        current_cluster = sigxy.cluster_id(current_index);
        cluster_route(total_count) = current_cluster;
    end
end