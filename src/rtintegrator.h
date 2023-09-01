#ifndef RTINTEGRATOR_H
#define RTINTEGRATOR_H



f64 PdfGetValue(pdf *Pdf, v3f64 w);
v3f64 PdfGenerate(pdf *Pdf);

//~ INITIALIZERS
pdf PdfCosineInit(pdf_kind Kind, v3f64 w)
{
  pdf Result = { .Kind = Kind, .Basis = OrthoNormBasisFromNormal(w) };
  return Result;
}
pdf PdfSurfaceInit(pdf_kind Kind, surface *Surface, v3f64 Origin)
{
  pdf Result = { .Kind = Kind, .Surface = Surface, .Origin = Origin };
  return Result;
}
pdf PdfMixInit(pdf_kind Kind, pdf *MixPdfA, pdf *MixPdfB)
{
  pdf Result = { .Kind = Kind, .MixPdf[0] = MixPdfA, .MixPdf[1] = MixPdfB };
  return Result;
}
pdf PdfInit(pdf_kind Kind, pdf *MixPdfA, pdf *MixPdfB, surface *Surface, v3f64 Origin, v3f64 w)
{
  pdf Result = {0};
  switch(Kind)
  {
    case PdfKind_Cosine: {
      Result = PdfCosineInit(Kind, w);
    } break;
    case PdfKind_Mixture: {
      Result = PdfMixInit(Kind, MixPdfA, MixPdfB);
    } break;
    case PdfKind_Surface: {
      Result = PdfSurfaceInit(Kind, Surface, Origin);
    } break;
  }
  return Result;
}

//~ GET VALUE
f64 PdfCosineGetValue(pdf *Pdf, v3f64 w)
{
  //listing 24
  f64 Cosine = Dot(Normalize(w), Pdf->Basis.w);
  f64 Result = (Cosine<0.0)?0.0:Cosine/Pi64;
  return Result;
}
f64 PdfSurfaceGetValue(pdf *Pdf, v3f64 w)
{
  f64 Result = SurfaceGetPdfValue(Pdf->Surface, Pdf->Origin, w);
  return Result;
}
f64 PdfMixtureGetValue(pdf *Pdf, v3f64 w)
{
  f64 Result = (0.5*PdfGetValue(Pdf->MixPdf[0], w) + 
                0.5*PdfGetValue(Pdf->MixPdf[1], w));
  return Result;
}
f64 PdfGetValue(pdf *Pdf, v3f64 w)
{
  f64 Result = 0.0;
  switch(Pdf->Kind)
  {
    case PdfKind_Cosine: {
      Result = PdfCosineGetValue(Pdf, w);
    } break;
    case PdfKind_Surface: {
      Result = PdfSurfaceGetValue(Pdf, w);
    } break;
    case PdfKind_Mixture: {
      Result = PdfMixtureGetValue(Pdf, w);
    } break;
  }
  return Result;
}

//~ GENERATE
v3f64 PdfCosineGenerate(pdf *Pdf)
{
  //listing 24
  v3f64 Result = OrthoNormBasisGetLocal(Pdf->Basis, RandCosineDir());
  return Result;
}
v3f64 PdfMixtureGenerate(pdf *Pdf)
{
  v3f64 Result = {0};
  f64 Random = RandF64Uni();
  if(Random<0.5)
  {
    Result = PdfGenerate(Pdf->MixPdf[0]);
  } else
  {
    Result = PdfGenerate(Pdf->MixPdf[1]);
  }
  return Result;
}
v3f64 PdfSurfaceGenerate(pdf *Pdf)
{
  v3f64 Result = SurfaceGenRandom(Pdf->Surface, Pdf->Origin);
  return Result;
}
v3f64 PdfGenerate(pdf *Pdf)
{
  v3f64 Result = {0};
  switch(Pdf->Kind)
  {
    case PdfKind_Cosine: {
      Result = PdfCosineGenerate(Pdf);
    } break;
    case PdfKind_Surface: {
      Result = PdfSurfaceGenerate(Pdf);
    } break;
    case PdfKind_Mixture: {
      Result = PdfMixtureGenerate(Pdf);
    } break;
  }
  return Result;
}


// Next Week
//TODO : listing 76 WORKING...

// Rest of your life
//TODO : listing 16 DONE!!!!
//TODO : listing 17 DONE!!!!
//TODO : listing 18 DONE!!!!
//TODO : listing 19
//TODO : listing 20
//TODO : listing 21 WORKING...
//TODO : listing 25


#endif //RTINTEGRATOR_H
