function [succ sigRoute clusterRoute coordRoute startCluster destCluster] = navigate(startCoord, destCoord, baseNumber, transHistory, centers, points, matrix, std_sig, distribution_table, base_number)
succ = 1;
successCnt = 0;
totalCnt = 0;
directionNumber = size(transHistory, 2);
currentCoord = startCoord;
currentSig = std_sig(std_sig(:,1) == startCoord(1) & std_sig(:,2) == startCoord(2), 5:2:end);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:baseNumber) = currentSig;
coordRoute(totalCnt, 1:2) = currentCoord;
currentCluster = get_cluster_sig(centers, currentSig, baseNumber, distribution_table);
startCluster = currentCluster;
clusterRoute(totalCnt) = currentCluster;

destSig = std_sig(std_sig(:,1) == destCoord(1) & std_sig(:,2) == destCoord(2), 5:2:end);
destCluster = get_cluster_sig(centers, destSig, baseNumber, distribution_table);

while 1
    if get_cluster_sig(centers, currentSig, baseNumber, distribution_table) == destCluster
        break;
    else
        [path direction] = guide(currentSig, destCluster, transHistory, centers, matrix, base_number, distribution_table);
        path
        direction
        if isempty(path)
            rand_direction = ceil(directionNumber*rand);
            currentCoord = get_next_step(rand_direction, currentCoord, points);
        else
            currentCoord = get_next_step(direction(1), currentCoord, points);
        end
        totalCnt = totalCnt + 1;
        if totalCnt > 300
            succ = 0;
            break;
        end
        currentSig = std_sig(std_sig(:,1) == currentCoord(1) & std_sig(:,2) == currentCoord(2), 5:2:end);
        sigRoute(totalCnt, 1:baseNumber) = currentSig;
        coordRoute(totalCnt, 1:2) = currentCoord;
        currentCluster = get_cluster_sig(centers, currentSig, baseNumber, distribution_table);
        clusterRoute(totalCnt) = currentCluster;
        if length(path) >= 1 && currentCluster == path(2)
            successCnt = successCnt + 1;
        end
    end
end