%% Initialization
len = 0;
error = 0;
e = 0;
d = 0;
%% Testing Main Loop
for j = 1:testing_round
    startX = path(j,1);
    startY = path(j,2);
    destX = path(j,3);
    destY = path(j,4);
    %% create matrix
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
    %% Navigate
    [succ sigRoute clusterRoute coordRoute startClus destClus] = navigate([startX, startY], [destX, destY], step_len, b, base_number, trans_history, center_sig, room, coefficient, matrix);
    if d == 0 && length(coordRoute) < 18 && length(coordRoute) > 1
        draw_path;
        d = 1;
    end
    if succ == 1
        len = len + length(clusterRoute)/((sum(([startX, startY]-[destX destY]).^2)).^.5);
        error = error + sum((coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        e = e + 1;
    end
end
error_rate = error / (testing_round-e);
step_ave = len / (testing_round-e);
error_count = e;

%measurements
time = [time step_ave]
accuracy = [accuracy error_rate]
error_time = [error_time e]
