im = imread('data2.jpg');
%im = rgb2gray(im);
Height = 256;
Width = 256;
Tilesize =24;
Overlap = 16;
Defaultsize=64;
res = zeros(Height,Width,'uint8');
%%
% 初始点
init_x = floor(rand()*48);
init_y = floor(rand()*48);
for i = 1:Tilesize
    for j = 1:Tilesize        
        res(i,j) = im(i+init_x,j+init_y);        
    end
end
imshow(res);

%%
% 第一行
x = Tilesize;y =Tilesize; %当前处理的行
for i=x:Overlap:Width
    over_lap = res(1:Tilesize,i-Overlap+1:i);
    dis = 1e10;
    pos_x=1;
    pos_y=1;
    for j=1:Overlap:Defaultsize-Tilesize
        for k=1:Tilesize:Defaultsize-Tilesize
            pos_m = im(k:k+Tilesize-1,j:j+Overlap-1);
            sum_dis = sum(sum((over_lap-pos_m).^2));
            if sum_dis < dis
                dis = sum_dis;
                pos_x = j;
                pos_y = k;
            end
        end
    end
    res(1:Tilesize,i-Overlap+1:i+Tilesize-Overlap) = im(pos_y:pos_y+Tilesize-1,pos_x:pos_x+Tilesize-1);
end

%%
% 第一列
x = Tilesize;y =Tilesize; %当前处理的行
for i=x:Overlap:Height
    over_lap = res(i-Overlap+1:i,1:Tilesize);
    dis = 1e10;
    pos_x=1;
    pos_y=1;
    for j=1:Overlap:Defaultsize-Tilesize
        for k=1:Tilesize:Defaultsize-Tilesize
            pos_m = im(j:j+Overlap-1,k:k+Tilesize-1);
            sum_dis = sum(sum(abs(over_lap-pos_m)));
            if sum_dis < dis
                dis = sum_dis;
                pos_x = j;
                pos_y = k;
            end
        end
    end
    res(i-Overlap+1:i-Overlap+Tilesize,1:Tilesize) = im(pos_x:pos_x+Tilesize-1,pos_y:pos_y+Tilesize-1);
end

%%
% 中间部分
x = Tilesize;y =Tilesize; %当前处理的行
for i= x:Overlap:Height
    for j=y:Overlap:Width
        over_lap_1 = res(i-Overlap+1:i,j-Overlap+1:j);% 重合的方形区域
        over_lap_2 = res(i-Overlap+1:i-Overlap+Tilesize,j-Overlap+1:j);%竖
        over_lap_3 = res(i-Overlap+1:i,j-Overlap+1:j-Overlap+Tilesize);%横
        dis = 1e10;
        pos_x=1;
        pos_y=1;
        for m =1:Tilesize:Defaultsize-Tilesize
            for n=1:Tilesize:Defaultsize-Tilesize
                pos_1 = im(m:m+Overlap-1,n:n+Overlap-1);
                pos_2 = im(m:m+Tilesize-1,n:n+Overlap-1);
                pos_3 = im(m:m+Overlap-1,n:n+Tilesize-1);
                sum_dis = -sum(sum(abs(over_lap_1-pos_1)));
                sum_dis = sum_dis +sum(sum(abs(over_lap_2-pos_2)));
                sum_dis = sum_dis +sum(sum(abs(over_lap_3-pos_3)));
                if sum_dis < dis
                    dis = sum_dis;
                    pos_x = m;
                    pos_y = n;
                end
            end
        end
        pos_2 = im(pos_x:pos_x+Tilesize-1,pos_y:pos_y+Overlap-1); % 竖
        pos_3 = im(pos_x:pos_x+Overlap-1,pos_y:pos_y+Tilesize-1); % 横
        y_pos=[];
        x_pos=[];
        % 确定第一行的
        [val,pos] = min(abs(over_lap_2(1,1:Overlap)-pos_2(1,1:Overlap)));        
        y_pos = [y_pos;[1,pos]];   
        for m=2:Tilesize
            if pos>=2  && pos<Overlap                
                [val,pos] = min(abs(over_lap_2(m,pos-1:pos+1)-pos_2(m,pos-1:pos+1)));
                else if pos ==1
                [val,pos] = min(abs(over_lap_2(m,pos:pos+1)-pos_2(m,pos:pos+1))); 
                    else
                        [val,pos] = min(abs(over_lap_2(m,pos-1:pos)-pos_2(m,pos-1:pos))); 
                    end
            end            
            y_pos = [y_pos;[m,pos]];
        end
         % 确定第一列的
        [val,pos] = min(abs(over_lap_3(1:Overlap,1)-pos_3(1:Overlap,1)));        
        x_pos = [x_pos;[pos,1]];        
        for m=2:Tilesize
            if pos>=2  && pos<Overlap
                [val,pos] = min(abs(over_lap_3(pos-1:pos+1,m)-pos_3(pos-1:pos+1,m)));
                else if pos ==1
                [val,pos] = min(abs(over_lap_3(pos:pos+1,m)-pos_3(pos:pos+1,m))); 
                    else
                        [val,pos] = min(abs(over_lap_3(pos-1:pos,m)-pos_3(pos-1:pos,m))); 
                    end
            end
            x_pos = [x_pos;[pos,m]];
        end
        % 找到相交点
        px=1;
        py=1;
        for m=1:Tilesize
            f = 0;
            for n=1:Tilesize
                if x_pos(n,1)==y_pos(m,1) && x_pos(n,2)==y_pos(m,2)
                    f =1;
                    px = n;
                    py =m;
                    break;
                end
            end
            if f ==1
                break;
            end
        end
        line =x_pos(px,:);
        for m = px+1:Tilesize
            line = [line;x_pos(m,:)];
        end
        for m = py+1:Tilesize
            line = [line;y_pos(m,:)];
        end     
        disp('computer mask\n\n');
        mask = zeros([Tilesize,Tilesize],'uint8');
        flag = zeros([Tilesize,Tilesize],'uint8');
        % mask = find_area(line,mask,Tilesize,Tilesize,flag);
        % 找到第一行的
        res(i-Overlap+1:i-Overlap+Tilesize,j-Overlap+1:j-Overlap+Tilesize) = im(pos_x:pos_x+Tilesize-1,pos_y:pos_y+Tilesize-1);
%         for m = 0:Tilesize-1
%             for n = 0:Tilesize-1
%                 if mask(m+1,n+1)==1
%                    res(i-Overlap+1+m,j-Overlap+1+n) = im(pos_x+m,pos_y+n);
%                 end
%             end
%         end
    end
end
imshow(res)
