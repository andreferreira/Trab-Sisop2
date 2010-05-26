package main

import (
  "fmt"
  "rand" 
  "time"
  "flag"
)

func main() {
	consumers := flag.Int("c", -1, "number of consumers, must be bigger than 0")
	producers := flag.Int("p", -1, "number of producers, must be bigger than 0")
	buffersize := flag.Int("b", -1, "size of buffer, must be bigger or equal to 0")
	flag.Parse()
	if *consumers <= 0 || *producers <= 0 || *buffersize < 0 {
		flag.PrintDefaults()
		return
	}
	canal := make(chan int, *buffersize)
	for i := 0; i < *producers; i++ {
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
	for i := 0; i < *consumers; i++ {
	  var threadnumber = i + *producers
	  go func () {
		for {
		  fmt.Printf("Thread %d recebeu %d\n",threadnumber,<-canal);
		  time.Sleep(rand.Int63n(5000))
		}
	  } ()
	}
	var quit = make(chan int)
	<- quit
} 
