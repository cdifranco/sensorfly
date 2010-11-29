function [path direction totalCost] = Guide(currentSig, destCluster, transHistory, transInitNumber, baseNumber,centers)

noOfNodes  = size(transHistory,1);

if destCluster > noOfNodes
    error('Point number is too large.');
else
    directionNumber = size(transHistory, 2);
    for i = 1:noOfNodes
        for j = 1:noOfNodes
            sumIJ = sum(transHistory(i,:,j));
            if sumIJ == transInitNumber*directionNumber
                matrix(i, j) = inf;
            else
                matrix(i, j) = sum(transHistory(i,:,j))/sum(sum(transHistory(i,:,:)));
                matrix(i, j) = 1/matrix(i, j);
            end;
        end;
    end;

    for i = 1:noOfNodes,
        % initialize the farthest node to be itself;
        farthestPreviousHop(i) = i;     % used to compute the RTS/CTS range;
        farthestNextHop(i) = i;
    end;

    startPoint = GetCluster(centers, currentSig);
    endPoint = destCluster;
    [path, totalCost] = Dijkstra(noOfNodes, matrix, startPoint, endPoint, farthestPreviousHop, farthestNextHop);
    
    %get direction
    direction = [];
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