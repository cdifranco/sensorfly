function [succ sigRoute clusterRoute coordRoute startCluster destCluster] = rand_go(startCoord, destCoord, stepLength, baseData, baseNumber, directionNumber, centers, room, coefficient)

succ = 1;
totalCnt = 0;
currentCoord = startCoord;
currentSig = convert(currentCoord(1),currentCoord(2), baseNumber, baseData, coefficient);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:baseNumber) = currentSig;
coordRoute(totalCnt, 1:2) = currentCoord;
currentCluster = GetCluster(centers, currentSig);
startCluster = currentCluster;
clusterRoute(totalCnt) = currentCluster;

destSig = convert(destCoord(1),destCoord(2), baseNumber, baseData, coefficient);
destCluster = GetCluster(centers, destSig);

while 1
    if GetCluster(centers, currentSig) == destCluster
        break;
    else
        rand_direction = ceil(directionNumber*rand);
        currentCoord = generate_next_step(directionNumber, directionNumber, rand_direction, stepLength, currentCoord(1),currentCoord(2),room);
        totalCnt = totalCnt + 1;
        if totalCnt > 300
            succ = 0;
            break;
        end
        currentSig = convert(currentCoord(1), currentCoord(2), baseNumber, baseData, coefficient);
        sigRoute(totalCnt, 1:baseNumber) = currentSig;
        coordRoute(totalCnt, 1:2) = currentCoord;
        currentCluster = GetCluster(centers, currentSig);
        clusterRoute(totalCnt) = currentCluster;
    end
end