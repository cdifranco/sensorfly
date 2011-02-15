function [path direction_order direction_count totalCost] = guide(currentCluster, destCluster, transHistory, count_to_id, matrix)

number_of_center  = size(matrix,1);
direction_count = [];
direction_order = [];
if destCluster > number_of_center
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, number_of_center);
    farthestNextHop = zeros(1, number_of_center);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:number_of_center) = 1:number_of_center;     % used to compute the RTS/CTS range;
    farthestNextHop(1:number_of_center) = 1:number_of_center;

    startPoint = currentCluster;
    endPoint = destCluster;
    [path, totalCost] = dijkstra(number_of_center, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    if ~isempty(path)
        [direction_count direction_order] = sort(transHistory(count_to_id(path(1)), :, count_to_id(path(2))),'descend');
    end
end