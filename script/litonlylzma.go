// Copyright 2022 The Wuffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//go:build ignore
// +build ignore

package main

// litonlylzma.go converts to and from the Literal Only LZMA compressed file
// format, a subset of the LZMA compressed file format (e.g. as spoken by
// /usr/bin/lzma on a Debian system).
//
// Usage:
//  go run litonlylzma.go -encode < foo.txt > foo.txt.litonlylzma
//  go run litonlylzma.go -decode < foo.txt.litonlylzma

import (
	"flag"
	"fmt"
	"io"
	"os"

	"github.com/google/wuffs/lib/litonlylzma"
)

var (
	decode = flag.Bool("decode", false, "decode from LZMA")
	encode = flag.Bool("encode", false, "encode to LZMA")
)

func main() {
	if err := main1(); err != nil {
		os.Stderr.WriteString(err.Error() + "\n")
		os.Exit(1)
	}
}

func main1() error {
	flag.Parse()
	if *decode == *encode {
		return fmt.Errorf("exactly one of -decode and -encode needed")
	}
	src, err := io.ReadAll(os.Stdin)
	if err != nil {
		return err
	}
	dst, err := []byte(nil), error(nil)
	if *decode {
		dst, _, err = litonlylzma.FileFormatLZMA.Decode(nil, src)
	} else {
		dst, err = litonlylzma.FileFormatLZMA.Encode(nil, src)
	}
	if err != nil {
		return err
	}
	_, err = os.Stdout.Write(dst)
	return err
}
