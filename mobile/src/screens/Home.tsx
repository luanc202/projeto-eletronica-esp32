import { Text, Center, Box, VStack, useToast } from "native-base";
import { useState } from "react";

import { Button } from "../components/Button";
import { api } from "../services/api";

export default function Home() {
  const [isLoading, setIsLoading] = useState(true);
  const [temp, setTemp] = useState({});

  const toast = useToast();

  async function handleGetTemperature() {
    try {
      setIsLoading(true);

      const response = await api.get('/temp');
      console.log(`log: ${response}`);
      
      setTemp(response);

      toast.show({
        title: 'Temperatura atualizada!',
        placement: 'top',
        bgColor: 'green.500'
      });

    } catch (error) {
      console.log(error);
      toast.show({
        title: 'Não foi possível atualizar temperatura.',
        placement: 'top',
        bgColor: 'red.500'
      })
    } finally {
      setIsLoading(false);
    }
  }

  return (
    <VStack
      flex={1}
      bg='gray.600'
    >
      <Text
        mt={32}
        color='white'
        fontSize={32}
        fontFamily='mono'
        textAlign='center'
      >
        App temperatura
      </Text>
      <Center alignItems='center' my={8}>
        <Text
          color='white'
          fontSize={20}
          fontFamily='bold'
          my={4}
        >
          {temp != '' ? `${temp}C°` : `Indefinido C°`}
        </Text>
        <Button
          title='Atualizar'
          textColor="white"
          bg='blue.700'
          onPress={handleGetTemperature}
        />
      </Center>
    </VStack>
  )
}