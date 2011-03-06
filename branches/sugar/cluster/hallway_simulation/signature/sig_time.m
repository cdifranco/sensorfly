%% get 4 reading for 5
sig1 = data(find(data(:,3)==5 & data(:,2)==1),:);
sig2 = data(find(data(:,3)==5 & data(:,2)==2),:);
sig3 = data(find(data(:,3)==5 & data(:,2)==3),:);
sig4 = data(find(data(:,3)==5 & data(:,2)==4),:);
plot(sig1(:,1)-201011050000000000,sig1(:,4),'Color','r');
hold on;
plot(sig2(:,1)-201011050000000000,sig2(:,4),'Color','g');
hold on;
plot(sig3(:,1)-201011050000000000,sig3(:,4),'Color','k');
hold on;
plot(sig4(:,1)-201011050000000000,sig4(:,4),'Color','b');
hold off;




