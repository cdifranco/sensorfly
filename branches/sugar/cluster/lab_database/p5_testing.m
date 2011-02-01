%% Initialization
load 'area_cluster.mat';
testing_round = 1000;
area_number = 12;
success = [];
observed_step = [];
error = [];
step = [];
%% Testing loop
for j = 1:testing_round
    dest_area = unidrnd(area_number);
    random_start_point = unidrnd(size(signatures, 1));
    current_point = signatures(random_start_point, 1:2);
     %% generate the matrix
    number_of_center = size(center_new,1);
    matrix = zeros(number_of_center, number_of_center);
    for l = 1:number_of_center
        for m = 1:number_of_center
            sumIJ = sum(trans_history(count_to_id(l),:,count_to_id(m)));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                sum_of_l_to_m = 0;
                for n = 1 : number_of_center
                    sum_of_l_to_m = sum_of_l_to_m + sum(trans_history(count_to_id(l),:,count_to_id(n)));
                end
                matrix(l, m) = sum(trans_history(count_to_id(l),:,count_to_id(m)))/sum_of_l_to_m;
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    %% call for navigate
    dest_location = get_area_location(dest_area);
    os = sqrt(sum((dest_location(:) - current_point(:)) .^ 2));
    [succ sigRoute e] = navigate(current_point, dest_area, base_number, trans_history, center_sig, count_to_id, matrix, area_cluster_relation, signatures);
     %% record metric
    fprintf('you have forwarded %d steps\n',size(sigRoute,1));
    if size(sigRoute,1) < 500
        success=  [success, 1];
        error = [error, e];
        step = [step, size(sigRoute,1)];
        observed_step = [observed_step, os];
    else
        success=  [success, 0];
        error = [error, inf];
        step = [step, inf];
        observed_step = [observed_step, inf];
    end
    save 'testing.mat';
end
step_rate = step(step(:)~=inf)/observed_step(observed_step(:)~=inf)*100
ave_error = sum(error(error(:)~=inf))/size(error(error(:)~=inf),2)
success_rate = sum(success)/size(success,2)
save 'testing.mat';