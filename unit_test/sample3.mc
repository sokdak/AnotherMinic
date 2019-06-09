def f(a,b)
{
  local c, d, e, f, g;

  if(a>b)
    max = a;
  else
    max = b;

  c = d = 3;
  e = c / d + 1;
    
  return max;
}

x = y = 1;

max = f(10,20);
print max;