def func1(a, b)
{
  local ret;

  while (a + b > 100)
  {
    a = a + 3;
    b = b - 1;
  }

  ret = a + b * 100;

  return ret;
}

def func2(a, b)
{
  local ret;

  if (a >= b)
    ret = 1;
  else
    ret = 0;

  return ret;
}

def func3(ab, cd) {
  local a;

  if (ab + cd >= 10000)
    a = (ab + cd) * 1.5;
  else
    a = func3(a, ab + cd);

  return a;
}

a = b = 10;
c = 1002.123;

res = func1(a, b);

print res;

res = func2(c, res);

print res;

res = func3(1, 1);

print res;