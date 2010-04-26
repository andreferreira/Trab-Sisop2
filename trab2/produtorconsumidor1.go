package main

import (
  "fmt"
  "rand" 
  "time"
)

func main() {
  var canal = make(chan int, 3)
  for i := 0; i < 2; i++ {
    var threadnumber = i
    go func () {
      for {
        var temp = rand.Intn(100);
        fmt.Printf("Thread %d enviando %d\n",threadnumber,temp);
        canal <- temp
        fmt.Printf("Thread %d enviado\n",threadnumber);
        time.Sleep(rand.Int63n(5000))
      } 
    } ()
  }
  var consumidor = func () {
    for {
      fmt.Printf("%d recebido\n",<-canal);
      time.Sleep(rand.Int63n(5000))
    }
  }
  consumidor()
} 
