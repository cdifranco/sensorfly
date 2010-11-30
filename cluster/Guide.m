function [path direction totalCost] = Guide(currentSig, destCluster, transHistory, transInitNumber, baseNumber,centers, matrix)

noOfNodes  = size(transHistory,1);

if destCluster > noOfNodes
    error('Point number is too large.');
else
    farthestPreviousHop = zeros(1, noOfNodes);
    farthestNextHop = zeros(1, noOfNodes);
    % initialize the farthest node to be itself;
    farthestPreviousHop(1:noOfNodes) = 1:noOfNodes;     % used to compute the RTS/CTS range;
    farthestNextHop(1:noOfNodes) = 1:noOfNodes;

    startPoint = GetCluster(centers, currentSig);
    endPoint = destCluster;
    [path, totalCost] = Dijkstra(noOfNodes, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    
    %get direction
    direction = zeros(1, length(path) - 1);
    for i = 1:length(path) - 1
        [maxNum direction(i)] = max(transHistory(path(i), :, path(i+1)));
    end
    %{
    if length(path) ~= 0
        for i = 1:(length(path)-1)
            line([netXloc(path(i)) netXloc(path(i+1))], [netYloc(path(i)) netYloc(path(i+1))], 'Color','r','LineWidth', 0.50, 'LineStyle', '-.');
       end;
    end;
    hold off;
    return;
    %}
    
    clear destPoint;
    clear directionNumber;
    clear farthestNextHop;
    clear farthestPreviousHop;
    clear i;
    clear j;
    clear matrix;
    clear maxNum
    clear startPoint;
end
clear noOfNodes;