def f(a,b)
{
  local max;

  if(a>b)
    max = a;
  else
    max = b;

  return max;
}

max = f(10,20);
print max;