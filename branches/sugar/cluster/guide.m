function [path direction_count direction_order totalCost] = guide(currentSig, destCluster, transHistory, centers, matrix)

noOfNodes  = size(transHistory,1);
direction_count = [];
direction_order = [];
if destCluster > noOfNodes
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, noOfNodes);
    farthestNextHop = zeros(1, noOfNodes);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:noOfNodes) = 1:noOfNodes;     % used to compute the RTS/CTS range;
    farthestNextHop(1:noOfNodes) = 1:noOfNodes;

    startPoint = get_cluster_sig(centers, currentSig);
    endPoint = destCluster;
    [path, totalCost] = dijkstra(noOfNodes, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    if ~isempty(path)
        [direction_count direction_order] = sort(transHistory(path(1), :, path(2)),'descend');
    end
end