%% Initialization
clear all;
close all;
test_type = 0;
while test_type ~= 1 && test_type ~= 2 && test_type ~= 3 && test_type ~= 4
    test_type = input('1--original; 2--kmeans convert; 3--kmeans new; 4--random');
end
if test_type == 1 || test_type == 4
	load 'area_cluster.mat';
elseif test_type == 2
    load 'kmeans_convert.mat';
    trans_history = trans_history_convert;
elseif test_type == 3
    load 'kmeans_new.mat';
    trans_history = trans_history_new;
end
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
     %% Generate the matrix
    number_of_center = size(center_new,1);
    matrix = zeros(number_of_center, number_of_center);
    if test_type == 1 || test_type == 2 %use old size trans_history table
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
    elseif test_type == 3 %use new size trans_history table
        for l = 1:number_of_center
            for m = 1:number_of_center
                sumIJ = sum(trans_history(l,:,m));
                if sumIJ == trans_init_number*direction_number
                    matrix(l, m) = inf;
                else
                    sum_of_l_to_m = 0;
                    for n = 1 : number_of_center
                        sum_of_l_to_m = sum_of_l_to_m + sum(trans_history(l,:,m));
                    end
                    matrix(l, m) = sum(trans_history(l,:,m))/sum_of_l_to_m;
                    matrix(l, m) = 1/matrix(l, m);
                end;
            end;
        end; 
    end
    %% Call for navigate
    dest_location = get_area_location(dest_area);
    os = sqrt(sum((dest_location(:) - current_point(:)) .^ 2));
    if test_type == 4 % random test
        [succ sigRoute e] = rand_go(current_point, dest_area, base_number, direction_number, center_sig, area_cluster_relation,  signatures);
    else
        [succ sigRoute e] = navigate(current_point, dest_area, base_number, trans_history, center_sig, count_to_id, matrix, area_cluster_relation, std_sig, distribution_table, test_type);
    end
     %% Record metric
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
end
step_rate = step(step(:)~=inf)/observed_step(observed_step(:)~=inf)*100
ave_error = sum(error(error(:)~=inf))/size(error(error(:)~=inf),2)
success_rate = sum(success)/size(success,2)
if test_type == 1
	save 'original_testing.mat';
elseif test_type == 2
    save 'kmeans_convert_testing.mat';
elseif test_type == 3
    save 'kmeans_new_testing.mat';
else 
    save 'random_testing.mat';
end