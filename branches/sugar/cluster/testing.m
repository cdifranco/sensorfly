len = 0;
error = 0;
e = 0;
packet_id = 0;

for j = 1:testing_round
    destArea = input('destiny area: ');
    %create matrix
    noOfNodes = size(trans_history,1);
    matrix = zeros(noOfNodes, noOfNodes);
    for l = 1:noOfNodes
        for m = 1:noOfNodes
            sumIJ = sum(trans_history(l,:,m));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                matrix(l, m) = sum(trans_history(l,:,m))/sum(sum(trans_history(l,:,:)));
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    [succ sigRoute clusterRoute coordRoute startClus destClus] = navigate_basic(packet_id, destArea, base_number, trans_history, center_sig, matrix);
    if succ == 1
        %TODO
        fprintf('u r in the dest area %d\n', destArea);
    else 
        e = e + 1;
    end
    stillcontinue = input('still continue? yes:1/no:0');
    if stillcontinue == 0
        break;
    end
end
