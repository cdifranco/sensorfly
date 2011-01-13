function [succ sigRoute startCluster destCluster] = navigate_basic(packet_id, port, reading, destArea, base_number, transHistory, centers, matrix)
succ = 1;
successCnt = 0;
totalCnt = 0;
directionNumber = size(transHistory, 2);
fprintf('read current signature');
[currentSig packet_id] = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:base_number) = currentSig;
currentCluster = get_cluster_sig(centers, currentSig)
startCluster = currentCluster;
destCluster = get_cluster_area(reading, destArea, size(centers,1))
while 1
    if get_cluster_sig(centers, currentSig) == destCluster
        fprintf('u r in the dest area %d\n', destArea);
        break;
    else
        [path direction_order] = guide(currentSig, destCluster, transHistory, centers, matrix);
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
        sigRoute(totalCnt, 1:base_number) = currentSig;
        currentCluster = get_cluster_sig(centers, currentSig);
        if length(path) >= 1 && currentCluster == path(2)
            successCnt = successCnt + 1;
        end
    end
end