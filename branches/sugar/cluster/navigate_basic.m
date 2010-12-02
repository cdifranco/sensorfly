function [succ sigRoute clusterRoute coordRoute startCluster destCluster] = navigate_basic(startCoord, destCoord, stepLength, baseData, baseNumber, transHistory, centers, room, coefficient, matrix)
succ = 1;
successCnt = 0;
totalCnt = 0;
cannotGoCnt = 0;
directionNumber = size(transHistory, 2);
currentCoord = startCoord;
currentSig = convert(currentCoord(1),currentCoord(2), baseNumber, baseData, coefficient);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:baseNumber) = currentSig;
coordRoute(totalCnt, 1:2) = currentCoord;
currentCluster = get_cluster(centers, currentSig);
startCluster = currentCluster;
clusterRoute(totalCnt) = currentCluster;

destSig = convert(destCoord(1),destCoord(2), baseNumber, baseData, coefficient);
destCluster = get_cluster(centers, destSig);

while 1
    if get_cluster(centers, currentSig) == destCluster
        break;
    else
        [path direction] = guide(currentSig, destCluster, transHistory, centers, matrix);
        if isempty(path)
            rand_direction = ceil(directionNumber*rand);
            currentCoord = generate_next_step(directionNumber, directionNumber, rand_direction, stepLength, currentCoord(1),currentCoord(2),room);
        else
            currentCoord = generate_next_step(directionNumber, directionNumber, direction(1), stepLength, currentCoord(1),currentCoord(2),room);
        end
        totalCnt = totalCnt + 1;
        if totalCnt > 300
            succ = 0;
            break;
        end
        currentSig = convert(currentCoord(1), currentCoord(2), baseNumber, baseData, coefficient);
        sigRoute(totalCnt, 1:baseNumber) = currentSig;
        coordRoute(totalCnt, 1:2) = currentCoord;
        currentCluster = get_cluster(centers, currentSig);
        clusterRoute(totalCnt) = currentCluster;
        if length(path) >= 1 && currentCluster == path(2)
            successCnt = successCnt + 1;
        end
    end
end