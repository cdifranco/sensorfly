% March clusters to 27 areas, since the areas are greographical unique, the
% cluster and area matching should be done separately
%load ('clustering.mat');
%% Area 1
area_cluster_relation{1} = get_cluster_in_one_area(400, 0, 480, 90, signatures, center_sig);
%% Area 2
area_cluster_relation{2} = get_cluster_in_one_area(300, 0, 390, 90, signatures, center_sig);
%% Area 3
area_cluster_relation{3} = get_cluster_in_one_area(200, 0, 290, 90, signatures, center_sig);
%% Area 4
area_cluster_relation{4} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 5
area_cluster_relation{5} = get_cluster_in_one_area(100, 100, 190, 190, signatures, center_sig);
%% Area 6
area_cluster_relation{6} = get_cluster_in_one_area(200, 100, 290, 190, signatures, center_sig);
%% Area 7
area_cluster_relation{7} = get_cluster_in_one_area(400, 100, 480, 190, signatures, center_sig);
%% Area 8
area_cluster_relation{8} = get_cluster_in_one_area(400, 200, 480, 290, signatures, center_sig);
%% Area 9
area_cluster_relation{9} = get_cluster_in_one_area(300, 200, 390, 290, signatures, center_sig);
%% Area 10
area_cluster_relation{10} = get_cluster_in_one_area(200, 200, 290, 290, signatures, center_sig);
%% Area 11
area_cluster_relation{11} = get_cluster_in_one_area(100, 200, 190, 290, signatures, center_sig);
%% Area 12
area_cluster_relation{12} = get_cluster_in_one_area(0, 200, 90, 290, signatures, center_sig);
%{
%% Area 13
area_cluster_relation{13} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 14
area_cluster_relation{14} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 15
area_cluster_relation{15} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 16
area_cluster_relation{16} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 17
area_cluster_relation{17} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 18
area_cluster_relation{18} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 19
area_cluster_relation{19} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 20
area_cluster_relation{20} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 21
area_cluster_relation{21} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 22
area_cluster_relation{22} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 23
area_cluster_relation{23} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 24
area_cluster_relation{24} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 25
area_cluster_relation{25} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 26
area_cluster_relation{26} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%% Area 27
area_cluster_relation{27} = get_cluster_in_one_area(100, 0, 190, 90, signatures, center_sig);
%}
save 'area_cluster.mat';
