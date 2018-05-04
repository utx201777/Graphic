function [mask] = find_area(line,mask,i,j,flag)        
    mask(i,j) = 1;   
    flag(i,j)=1;
    next = [-1,0;0,-1];
    %fprintf('next %d %d\n\n',i,j);
    for m =1:2
        x = i+next(m,1);
        y = j+next(m,2);
        if x<1 || y<1 || flag(x,y)==1
            continue;
        end
        f = 0;
        for n = 1:size(line,1)
            if line(n,1)==x && line(n,2)==y
                f =1;
                break;
            end
        end
        if f==0            
            mask = find_area(line,mask,x,y,flag);
        end
    end    
end