rand_l = 0;
rand_error = 0;
rand_e = 0;
rand_testing_round = 100;
for j = 1:rand_testing_round
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
    
    [rand_succ rand_sigRoute rand_clusterRoute rand_coordRoute rand_startClus rand_destClus] = RandGo([startX, startY], [destX, destY], step_len, b, base_number, trans_history, trans_init_number, center, room, coefficient);
    if rand_succ == 1
        l = l + length(clusterRoute)/sum(([startX, startY]-[destX destY]).^2).^.5;
        rand_error = rand_error + sum((rand_coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        rand_e = rand_e + 1;
        rand_errRoute{e} = rand_clusterRoute;
    end
end
rand_error_rate = rand_error / (rand_testing_round-rand_e)
rand_step_ave = l / (rand_testing_round-rand_e)
%rand_error_count = e
time = [time rand_step_ave]
accuracy = [accuracy rand_error_rate]