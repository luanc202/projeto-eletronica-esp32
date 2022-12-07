import { Text, Center, Switch, VStack, useToast } from "native-base";
import { useState } from "react";

import { Button } from "../components/Button";
import { api } from "../services/api";

export default function Home() {
  const [isLoading, setIsLoading] = useState(false);
  const [temp, setTemp] = useState({});
  const [isRelayOn, setIsRelayOn] = useState('');

  const toast = useToast();

  async function handleGetRelay() {
    try {
      setIsLoading(true);

      const response = await api.get('/rele');
      console.log(`log: ${response.data}`);
      const text = String(response.data).toUpperCase();

      setIsRelayOn(text);

      toast.show({
        // title: `Relé ${response == 'ON' ? 'ligado' : 'desligado'}.`,
        placement: 'top',
        bgColor: 'green.500'
      });

    } catch (error) {
      console.log(error);
      toast.show({
        title: 'Não foi possível atualizar relé.',
        placement: 'top',
        bgColor: 'red.500'
      })
    } finally {
      setIsLoading(false);
    }
  }

  async function handleGetTemperature() {
    try {
      setIsLoading(true);

      const response = await api.get('/');
      console.log(`log: ${response.data}`);
      const text = String(response.data);

      setTemp(text);

      toast.show({
        title: `Temperatura atualizada para ${text}C°.`,
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
        fontWeight='extrabold'
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
          isLoading={isLoading}
        />
        <Text
          color='white'
          fontSize={20}
          fontFamily='bold'
          my={4}
        >Ligar/Desligar relé</Text>
        <Switch size='lg' colorScheme="emerald" onToggle={handleGetRelay} />
        <Text
          color='white'
          fontSize={20}
          fontFamily='bold'
          my={4}
        >{isRelayOn}</Text>
      </Center>
    </VStack>
  )
}