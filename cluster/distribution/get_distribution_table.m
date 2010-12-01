function distribution_table = get_distribution_table(base_num, rang_dist_itvl, noise_rate)
%% randomly locate base stations to a base_num*2 matrix
base_coord = zeros(base_num, 2);
for i = 1:base_num
    base_coord(i, 1) = 3*rand;
    if base_coord(i, 1) <= 1
        base_coord(i, 2) = 4*rand;
    else
        base_coord(i, 2) = 9*rand;
    end
end

%% get signatures of even-distributed points in a circle with r meter radius
load real_rang_dist;
center_coord = [1.5, 4.5];
sig_num_per_point = 15;
radius = 0.7;
real_radius_itvl = 0.1;
real_radius_point_num = radius/real_radius_itvl+1
real_angle_point_num = 10;
real_angle_itvl = 2*pi/real_angle_point_num;
sig_map = zeros(real_radius_point_num, real_angle_point_num, sig_num_per_point, base_num);
for i = 1:real_radius_point_num
    for j = 1:real_angle_point_num
        current_coord = [center_coord(1) + real_radius_itvl*(i-1)*cos(2*pi*j/real_angle_itvl), center_coord(2) + real_radius_itvl*(i-1)*sin(2*pi*j/real_angle_itvl)];
        for l = 1:base_num
            real_dist = sum((current_coord - base_coord(l, :)).^2)^.5;
            for k = 1:sig_num_per_point
                sig_map(i, j, k, l) = polyval(coefficient, real_dist) * (1 + (2*rand - 1) * noise_rate);
            end
        end
    end
end

%% get distribution table
total_item_num = (radius/real_radius_itvl+1)*(((2*pi/real_angle_itvl)*sig_num_per_point)^2);
distance_table = zeros(total_item_num, 2);
item_num = 0;
for i = 1:real_radius_point_num
    real_dist = real_radius_itvl*(i-1);
    for j = 1:real_angle_point_num
        for k = 1:sig_num_per_point
            for l = 1:real_angle_point_num
                for m = 1:sig_num_per_point
                    sig1 = sig_map(1, j, k, :);
                    sig2 = sig_map(i, l, m, :);
                    sig_dist = sum((sig1 - sig2).^2)^.5;
                    item_num = item_num + 1;
                    distance_table(item_num, 1:2) = [real_dist sig_dist];
                    %item_num/total_item_num
                end
            end
        end
    end
end

%% normalization
min_val = min(distance_table);
min_val = min_val(2) - 0.000000000000001;
max_val = max(distance_table);
max_val = max_val(2);

rang_dist = linspace(min_val, max_val, ceil((max_val - min_val)/rang_dist_itvl)+1);
distribution_table = zeros(size(rang_dist, 2)-1, 4);
for i = 1:size(distribution_table, 1)
    distribution_table(i, 1:2) = [rang_dist(i) rang_dist(i+1)];
end

for i = 1:size(distance_table, 1)
    rang_itvl_num = ceil((distance_table(i, 2) - min_val)/rang_dist_itvl);
    distribution_table(rang_itvl_num, 3) = distribution_table(rang_itvl_num, 3) + 1;
end

max_cnt = max(distribution_table(:,3));
for i = 1:size(distribution_table, 1)
    distribution_table(i, 4) = distribution_table(i, 3)/max_cnt;
end

distribution_table = [distribution_table; distribution_table(end, 2) 100 0 0];
distribution_table = [distribution_table; 100 100.01 0 0];

clear i;