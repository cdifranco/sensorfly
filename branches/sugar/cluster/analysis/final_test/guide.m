function [path direction totalCost] = guide(currentSig, destCluster, transHistory, centers, matrix, base_number, distribution_table)

noOfNodes  = size(transHistory,1);

if destCluster > noOfNodes
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, noOfNodes);
    farthestNextHop = zeros(1, noOfNodes);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:noOfNodes) = 1:noOfNodes;     % used to compute the RTS/CTS range;
    farthestNextHop(1:noOfNodes) = 1:noOfNodes;

    startPoint = get_cluster_sig(centers, currentSig, base_number, distribution_table);
    endPoint = destCluster;
    [path, totalCost] = dijkstra(noOfNodes, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    
    % get direction
    direction = zeros(1, length(path) - 1);
    for i = 1:length(path) - 1
        [~, direction(i)] = max(transHistory(path(i), :, path(i+1)));
    end
end