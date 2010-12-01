len = 0;
error = 0;
e = 0;
testing_round = 100;

for j = 1:testing_round
    %percentage = j
    startX = 3*rand;
    if startX <= 1
        startY = 4*rand;
    else
        startY = 9*rand;
    end
    destX = 3*rand;
    if destX <= 1
        destY = 4*rand;
    else
        destY = 9*rand;
    end
    
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
    [succ sigRoute clusterRoute coordRoute startClus destClus] = navigate([startX, startY], [destX, destY], step_len, b, base_number, trans_history, center_sig, room, coefficient, matrix);
    if succ == 1
        len = len + length(clusterRoute)/((sum(([startX, startY]-[destX destY]).^2)).^.5);
        error = error + sum((coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        e = e + 1;
    end
end
error_rate = error / (testing_round-e);
step_ave = len / (testing_round-e);
%error_count = e;

%measurements
time = [time step_ave]
accuracy = [accuracy error_rate]
%center_count = [center_count size(center, 1)]
%error_time = [error_time e]
