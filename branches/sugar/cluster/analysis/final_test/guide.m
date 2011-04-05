function [path direction totalCost] = guide(current_index, dest_cluster, trans_history, matrix, sigxy)

noOfNodes  = size(trans_history,1);

if dest_cluster > noOfNodes
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, noOfNodes);
    farthestNextHop = zeros(1, noOfNodes);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:noOfNodes) = 1:noOfNodes;     % used to compute the RTS/CTS range;
    farthestNextHop(1:noOfNodes) = 1:noOfNodes;

    startPoint = sigxy.cluster_id(current_index);
    endPoint = dest_cluster;
    [path, totalCost] = dijkstra(noOfNodes, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    
    % get direction
    direction = zeros(1, length(path) - 1);
    for i = 1:length(path) - 1
        [~, direction(i)] = max(trans_history(path(i), :, path(i+1)));
    end
end