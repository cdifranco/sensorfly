function [path direction_order direction_count total_cost] = guide(current_cluster, dest_cluster, trans_history, count_to_id, matrix, test_type)

number_of_center  = size(matrix,1);
direction_count = [];
direction_order = [];
if dest_cluster > number_of_center
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, number_of_center);
    farthestNextHop = zeros(1, number_of_center);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:number_of_center) = 1:number_of_center;     
    farthestNextHop(1:number_of_center) = 1:number_of_center;

    startPoint = current_cluster;
    endPoint = dest_cluster;
    [path, total_cost] = dijkstra(number_of_center, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    if ~isempty(path)
        if test_type == 1 || test_type == 2
            [direction_count direction_order] = sort(trans_history(count_to_id(path(1)), :, count_to_id(path(2))),'descend');
        else
            [direction_count direction_order] = sort(trans_history(path(1), :, path(2)),'descend');
        end
    end
end