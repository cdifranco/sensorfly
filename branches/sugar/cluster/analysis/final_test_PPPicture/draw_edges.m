%for hallway

line([0 100], [400 400], 'Color', 'k');
line([100 100], [400 900], 'Color', 'k');
vm = text(40, 420, '{\fontsize{15} Vending Machine}');
set(vm, 'rotation', 90);
xlabel('room width (cm)');
ylabel('room length (cm)');

%for lab
%{
line([0 100], [200 200], 'Color', 'k');
line([100 100], [0 200], 'Color', 'k');
line([280 280], [90 170], 'Color', 'k');
line([280 290], [170 170], 'Color', 'k');
line([290 290], [170 190], 'Color', 'k');
line([290 380], [190 190], 'Color', 'k');
line([380 380], [190 160], 'Color', 'k');
line([380 390], [160 160], 'Color', 'k');
line([390 390], [160 90], 'Color', 'k');
line([390 280], [90 90], 'Color', 'k');
line([280 280], [390 270], 'Color', 'k');
line([280 350], [270 270], 'Color', 'k');
line([350 350], [270 290], 'Color', 'k');
line([350 480], [290 290], 'Color', 'k');
text(5, 100, '{\fontsize{20} tables}');
text(300, 130, '{\fontsize{20} pillar}');
text(320, 350, '{\fontsize{20} tables}');
%}