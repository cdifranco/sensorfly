function [succ sigRoute clusterRoute coordRoute startCluster destCluster] = RandGo(startCoord, destCoord, stepLength, baseData, baseNumber, transHistory, transInitNumber, centers)

succ = 1;
successCnt = 0;
totalCnt = 0;
cannotGoCnt = 0;
directionNumber = size(transHistory, 2);
currentCoord = startCoord;
currentSig = convert(currentCoord(1),currentCoord(2), baseNumber, baseData);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:baseNumber) = currentSig;
coordRoute(totalCnt, 1:2) = currentCoord;
currentCluster = GetCluster(centers, currentSig);
startCluster = currentCluster;
clusterRoute(totalCnt) = currentCluster;

destSig = convert(destCoord(1),destCoord(2), baseNumber, baseData);
destCluster = GetCluster(centers, destSig);

while 1
    if GetCluster(centers, currentSig) == destCluster
        break;
    else
        angle = 2*pi*rand;
        newX = currentCoord(1) + stepLength*cos(angle);
        newY = currentCoord(2) + stepLength*sin(angle);
        if newX < 1
            if newY > 4
                newY = 8 - newY;
            end
        else
            if newY > 9
                newY = 18 - newY;
            end  
        end
        if newY < 0
            newY = 0 - newY;
        end  

        if newY < 4
            if newX < 0 
                newX = 0 - newX;
            elseif newX > 3
                newX = 6 - newX;
            end
        else
            if newX < 1 
                newX = 2 - newX;
            elseif newX > 3
                newX = 6 - newX;
            end
        end
        totalCnt = totalCnt + 1;
        if totalCnt > 300
            succ = 0;
            break;
        end
        currentCoord = [newX newY];
        currentSig = convert(currentCoord(1), currentCoord(2), baseNumber, baseData);
        sigRoute(totalCnt, 1:baseNumber) = currentSig;
        coordRoute(totalCnt, 1:2) = currentCoord;
        currentCluster = GetCluster(centers, currentSig);
        clusterRoute(totalCnt) = currentCluster;
    end
end