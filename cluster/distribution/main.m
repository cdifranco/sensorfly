for i = 1:20
    distribution_table{i} = get_distribution_table(i, 0.7, 0.3);
    fprintf('i = %d completed.\n', i);
end
clear i;