function [succ sigRoute clusterRoute coordRoute startCluster destCluster] = navigate_basic(packet_id, reading, destArea, baseNumber, transHistory, centers, matrix)
succ = 1;
successCnt = 0;
totalCnt = 0;
directionNumber = size(transHistory, 2);
currentArea = startArea;
fprintf('read current signature');
currentSig = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:baseNumber) = currentSig;
coordRoute(totalCnt, 1:2) = currentArea;
currentCluster = get_cluster_sig(centers, currentSig);
startCluster = currentCluster;
clusterRoute(totalCnt) = currentCluster;
destCluster = get_cluster_area(reading, destArea, size(centers,1));
while 1
    if get_cluster(centers, currentSig) == destCluster
        break;
    else
        [path direction_order] = guide(reading, currentArea, destCluster, transHistory, centers, matrix);
        fprintf('go: ');
        if isempty(path)
            ceil(directionNumber*rand)
        else
            direction_order(1)
        end
        fprintf('\n');
        stillcontinue = input('continue?(yes:1/no:0)');
        if stillcontinue == 0
            break;
        end
        currentSig = get_sig_from_port(packet_id, port, base_number);
        sigRoute(totalCnt, 1:baseNumber) = currentSig;
        coordRoute(totalCnt, 1:2) = currentCoord;
        currentCluster = get_cluster_sig(centers, currentSig);
        clusterRoute(totalCnt) = currentCluster;
        if length(path) >= 1 && currentCluster == path(2)
            successCnt = successCnt + 1;
        end
    end
end